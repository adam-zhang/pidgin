/*
 * libyay
 *
 * Copyright (C) 2001 Eric Warmenhoven <warmenhoven@yahoo.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "internal.h"

void (*yahoo_socket_notify)(struct yahoo_session *, int, int, gboolean) = NULL;
void (*yahoo_print)(struct yahoo_session *, int, const char *) = NULL;
int (*yahoo_connector)(struct yahoo_session *, const char *, int, gpointer) = NULL;

struct yahoo_session *yahoo_new()
{
	struct yahoo_session *sess;

	if (!(sess = g_new0(struct yahoo_session, 1)))
		return NULL;

	return sess;
}

void yahoo_set_proxy(struct yahoo_session *session, int proxy_type, char *proxy_host, int proxy_port)
{
	if (!session || !proxy_type || !proxy_host)
		return;

	session->proxy_type = proxy_type;
	session->proxy_host = g_strdup(proxy_host);
	session->proxy_port = proxy_port;
}

static int yahoo_connect_host(struct yahoo_session *sess, const char *host, int port, int *statusret)
{
	struct sockaddr_in sa;
	struct hostent *hp;
	int fd;

	if (!(hp = gethostbyname(host))) {
		YAHOO_PRINT(sess, YAHOO_LOG_WARNING, "Resolve error");
		if (statusret)
			*statusret = (h_errno | YAHOO_CONN_STATUS_RESOLVERR);
		return -1;
	}

	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_port = htons(port);
	memcpy(&sa.sin_addr, hp->h_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;

	fd = socket(hp->h_addrtype, SOCK_STREAM, 0);

	fcntl(fd, F_SETFL, O_NONBLOCK);

	if (connect(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) < 0) {
		if ((errno == EINPROGRESS) || (errno == EINTR)) { 
			YAHOO_PRINT(sess, YAHOO_LOG_NOTICE, "Connect would block");
			if (statusret)
				*statusret |= YAHOO_CONN_STATUS_INPROGRESS;
			return fd;
		}
		close(fd);
		fd = -1;
	}

	return fd;
}

int yahoo_connected(struct yahoo_session *session, gpointer data, int fd)
{
	struct yahoo_conn *conn = data;

	if (fd < 0) {
		YAHOO_PRINT(session, YAHOO_LOG_WARNING, "connect failed");
		session->connlist = g_list_remove(session->connlist, conn);
		g_free(conn);
		return 0;
	}

	YAHOO_PRINT(session, YAHOO_LOG_NOTICE, "connect succeeded");
	conn->socket = fd;
	conn->connected = TRUE;
	if (conn->type == YAHOO_CONN_TYPE_AUTH) {
		if (session->callbacks[YAHOO_HANDLE_AUTHCONNECT].function)
			(*session->callbacks[YAHOO_HANDLE_AUTHCONNECT].function)(session);
	} else if (conn->type == YAHOO_CONN_TYPE_MAIN) {
		if (session->callbacks[YAHOO_HANDLE_MAINCONNECT].function)
			(*session->callbacks[YAHOO_HANDLE_MAINCONNECT].function)(session);
	} else if (conn->type == YAHOO_CONN_TYPE_DUMB) {
		YAHOO_PRINT(session, YAHOO_LOG_DEBUG, "sending to buddy list host");
		yahoo_write(session, conn, conn->txqueue, strlen(conn->txqueue));
		g_free(conn->txqueue);
		conn->txqueue = NULL;
	}

	if (yahoo_socket_notify)
		(*yahoo_socket_notify)(session, conn->socket, YAHOO_SOCKET_READ, TRUE);
	else
		YAHOO_PRINT(session, YAHOO_LOG_CRITICAL, "yahoo_socket_notify not set up");

	return 1;
}

struct yahoo_conn *yahoo_new_conn(struct yahoo_session *session, int type, const char *host, int port)
{
	struct yahoo_conn *conn;
	int status;

	if (!session)
		return NULL;

	conn = g_new0(struct yahoo_conn, 1);
	conn->type = type;

	if (yahoo_connector) {
		YAHOO_PRINT(session, YAHOO_LOG_DEBUG, "Connecting using user-specified connect routine");
		if ((*yahoo_connector)(session, host, port, conn) < 0) {
			YAHOO_PRINT(session, YAHOO_LOG_CRITICAL, "connect failed");
			g_free(conn);
			return NULL;
		}
		session->connlist = g_list_append(session->connlist, conn);
		return conn;
	}

	if (session->proxy_type) {
		YAHOO_PRINT(session, YAHOO_LOG_DEBUG, "connecting to proxy");
		conn->socket = yahoo_connect_host(session, session->proxy_host,
				session->proxy_port, &status);
		if (type == YAHOO_CONN_TYPE_MAIN)
			conn->type = YAHOO_CONN_TYPE_PROXY;
	} else
		conn->socket = yahoo_connect_host(session, host, port, &status);

	if (conn->socket < 0) {
		g_free(conn);
		return NULL;
	}

	if (yahoo_socket_notify)
		(*yahoo_socket_notify)(session, conn->socket, YAHOO_SOCKET_WRITE, TRUE);
	else
		YAHOO_PRINT(session, YAHOO_LOG_CRITICAL, "yahoo_socket_notify not set up");

	session->connlist = g_list_append(session->connlist, conn);

	return conn;
}

struct yahoo_conn *yahoo_getconn_type(struct yahoo_session *sess, int type)
{
	GList *c;
	struct yahoo_conn *conn;

	if (!sess)
		return NULL;

	c = sess->connlist;
	while (c) {
		conn = c->data;
		if (conn->type == type)
			return conn;
		c = g_list_next(c);
	}

	return NULL;
}

struct yahoo_conn *yahoo_find_conn(struct yahoo_session *sess, int socket)
{
	GList *c;
	struct yahoo_conn *conn;

	c = sess->connlist;
	while (c) {
		conn = c->data;
		if (conn->socket == socket)
			return conn;
		c = g_list_next(c);
	}

	return NULL;
}

int yahoo_connect(struct yahoo_session *session, const char *host, int port)
{
	if (!session)
		return 0;

	if (session->auth_host)
		g_free(session->auth_host);
	if (host && *host)
		session->auth_host = g_strdup(host);
	else
		session->auth_host = g_strdup(YAHOO_AUTH_HOST);

	if (port)
		session->auth_port = port;
	else
		session->auth_port = YAHOO_AUTH_PORT;

	if (!yahoo_new_conn(session, YAHOO_CONN_TYPE_AUTH, session->auth_host, session->auth_port))
		return 0;

	return 1;
}

int yahoo_major_connect(struct yahoo_session *session, const char *host, int port)
{
	if (!session)
		return 0;

	if (session->pager_host)
		g_free(session->pager_host);
	if (host && *host)
		session->pager_host = g_strdup(host);
	else
		session->pager_host = g_strdup(YAHOO_PAGER_HOST);

	if (port)
		session->pager_port = port;
	else
		session->pager_port = YAHOO_AUTH_PORT;

	if (!yahoo_new_conn(session, YAHOO_CONN_TYPE_MAIN, session->pager_host, session->pager_port))
		return 0;

	return 1;
}

void yahoo_close(struct yahoo_session *session, struct yahoo_conn *conn)
{
	if (!session || !conn)
		return;

	if (!g_list_find(session->connlist, conn))
		return;

	if (yahoo_socket_notify) {
		if (conn->connected)
			(*yahoo_socket_notify)(session, conn->socket, YAHOO_SOCKET_READ, FALSE);
		else
			(*yahoo_socket_notify)(session, conn->socket, YAHOO_SOCKET_WRITE, FALSE);
	}
	close(conn->socket);

	session->connlist = g_list_remove(session->connlist, conn);
	if (conn->txqueue)
		g_free(conn->txqueue);
	g_free(conn);
}

int yahoo_disconnect(struct yahoo_session *session)
{
	if (!session)
		return 0;
	if (session->name)
		g_free(session->name);
	yahoo_logoff(session);
	session->name = NULL;
	while (session->connlist)
		yahoo_close(session, session->connlist->data);
	if (session->cookie)
		g_free(session->cookie);
	session->cookie = NULL;
	if (session->login_cookie)
		g_free(session->login_cookie);
	session->login_cookie = NULL;
	while (session->ignored) {
		g_free(session->ignored->data);
		session->ignored = g_list_remove(session->ignored, session->ignored->data);
	}
	if (session->identities)
		g_strfreev(session->identities);
	session->identities = NULL;
	if (session->login)
		g_free(session->login);
	session->login = NULL;
	while (session->groups) {
		struct yahoo_group *grp = session->groups->data;
		g_strfreev(grp->buddies);
		g_free(grp->name);
		g_free(grp);
		session->groups = g_list_remove(session->groups, grp);
	}
	if (session->auth_host)
		g_free(session->auth_host);
	session->auth_host = NULL;
	if (session->pager_host)
		g_free(session->pager_host);
	session->pager_host = NULL;
	return 0;
}

int yahoo_delete(struct yahoo_session *session)
{
	if (!session)
		return 0;
	if (session->proxy_host)
		g_free(session->proxy_host);
	g_free(session);
	return 0;
}
