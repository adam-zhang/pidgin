/*
 * IPC test client plugin.
 *
 * Copyright (C) 2003 Christian Hammond.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */
#include "internal.h"
#include "debug.h"
#include "plugin.h"

#define IPC_TEST_CLIENT_PLUGIN_ID "core-ipc-test-client"

static gboolean
plugin_load(GaimPlugin *plugin)
{
	GaimPlugin *server_plugin;
	gboolean ok;
	int result;

	server_plugin = gaim_plugins_find_with_id("core-ipc-test-server");

	if (server_plugin == NULL)
	{
		gaim_debug_error("ipc-test-client",
						 "Unable to locate plugin core-ipc-test-server, "
						 "needed for IPC.\n");

		return TRUE;
	}

	result = (int)gaim_plugin_ipc_call(server_plugin, "add", &ok, 36, 6);

	if (!ok)
	{
		gaim_debug_error("ipc-test-client",
						 "Unable to call IPC function 'add' in "
						 "core-ipc-test-server plugin.");

		return TRUE;
	}

	gaim_debug_info("ipc-test-client", "36 + 6 = %d\n", result);

	result = (int)gaim_plugin_ipc_call(server_plugin, "sub", &ok, 50, 8);

	if (!ok)
	{
		gaim_debug_error("ipc-test-client",
						 "Unable to call IPC function 'sub' in "
						 "core-ipc-test-server plugin.");

		return TRUE;
	}

	gaim_debug_info("ipc-test-client", "50 - 8 = %d\n", result);

	return TRUE;
}

static GaimPluginInfo info =
{
	GAIM_PLUGIN_API_VERSION,                          /**< api_version    */
	GAIM_PLUGIN_STANDARD,                             /**< type           */
	NULL,                                             /**< ui_requirement */
	0,                                                /**< flags          */
	NULL,                                             /**< dependencies   */
	GAIM_PRIORITY_DEFAULT,                            /**< priority       */

	IPC_TEST_CLIENT_PLUGIN_ID,                        /**< id             */
	N_("IPC Test Client"),                            /**< name           */
	VERSION,                                          /**< version        */
	                                                  /**  summary        */
	N_("Test plugin IPC support, as a client."),
	                                                  /**  description    */
	N_("Test plugin IPC support, as a client. This locates the server "
	   "plugin and calls the commands registered."),
	"Christian Hammond <chipx86@gnupdate.org>",       /**< author         */
	GAIM_WEBSITE,                                     /**< homepage       */

	plugin_load,                                      /**< load           */
	NULL,                                             /**< unload         */
	NULL,                                             /**< destroy        */

	NULL,                                             /**< ui_info        */
	NULL                                              /**< extra_info     */
};

static void
init_plugin(GaimPlugin *plugin)
{
	info.dependencies = g_list_append(info.dependencies,
									  "core-ipc-test-server");
}

GAIM_INIT_PLUGIN(ipctestclient, init_plugin, info)
