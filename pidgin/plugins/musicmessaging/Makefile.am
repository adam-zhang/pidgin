EXTRA_DIST = \
	music.png

musicmessagingdir = @PIDGIN_PLUGINDIR@

musicmessaging_la_LDFLAGS = -module @PLUGIN_LDFLAGS@

if PLUGINS
if ENABLE_DBUS

musicmessagingpixdir = $(datadir)/pixmaps/pidgin/buttons
musicmessagingpix_DATA = music.png

musicmessaging_LTLIBRARIES = musicmessaging.la

musicmessaging_la_SOURCES = \
	musicmessaging.c

musicmessaging_la_LIBADD = @PIDGIN_LIBS@ $(DBUS_LIBS)

CLEANFILES              = music-messaging-bindings.ch

.PHONY: always

$(top_builddir)/libpurple/dbus-types.h: always
	$(AM_V_GEN)cd $(@D) && $(MAKE) $(AM_MAKEFLAGS) $(@F)

music-messaging-bindings.ch: $(top_srcdir)/libpurple/dbus-analyze-functions.py $(musicmessaging_la_SOURCES)
	$(AM_V_GEN)cat $(srcdir)/$(musicmessaging_la_SOURCES) | \
	$(PYTHON) $(top_srcdir)/libpurple/dbus-analyze-functions.py --export-only > $@

$(musicmessaging_la_OBJECTS) musicmessaging.so: music-messaging-bindings.ch $(top_builddir)/libpurple/dbus-types.h

endif
endif

AM_CPPFLAGS = \
	-I$(top_builddir)/libpurple \
	-I$(top_srcdir)/libpurple \
	-I$(top_srcdir)/pidgin \
	$(DEBUG_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GPLUGIN_CFLAGS) \
	$(DBUS_CFLAGS)
