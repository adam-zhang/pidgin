DIST_SUBDIRS = cap disco gestures gevolution musicmessaging ticker win32/winprefs win32/transparency

if BUILD_GEVOLUTION
GEVOLUTION_DIR = gevolution
endif

if ENABLE_DBUS
MUSICMESSAGING_DIR = musicmessaging
endif

if ENABLE_CAP
CAP_DIR = cap
endif

if ENABLE_GESTURES
GESTURE_DIR = gestures
endif

SUBDIRS = \
	$(CAP_DIR) \
	$(GESTURE_DIR) \
	$(GEVOLUTION_DIR) \
	$(MUSICMESSAGING_DIR) \
	disco \
	ticker

if IS_WIN32
SUBDIRS += \
	win32/winprefs \
	win32/transparency
endif

plugindir = @PIDGIN_PLUGINDIR@

contact_priority_la_LDFLAGS = -module @PLUGIN_LDFLAGS@
extplacement_la_LDFLAGS     = -module @PLUGIN_LDFLAGS@
gtk_signals_test_la_LDFLAGS = -module @PLUGIN_LDFLAGS@
gtkbuddynote_la_LDFLAGS     = -module @PLUGIN_LDFLAGS@
history_la_LDFLAGS          = -module @PLUGIN_LDFLAGS@
iconaway_la_LDFLAGS         = -module @PLUGIN_LDFLAGS@
imgupload_la_LDFLAGS        = -module @PLUGIN_LDFLAGS@
markerline_la_LDFLAGS       = -module @PLUGIN_LDFLAGS@
notify_la_LDFLAGS           = -module @PLUGIN_LDFLAGS@
relnot_la_LDFLAGS           = -module @PLUGIN_LDFLAGS@
screencap_la_LDFLAGS        = -module @PLUGIN_LDFLAGS@
sendbutton_la_LDFLAGS       = -module @PLUGIN_LDFLAGS@
spellchk_la_LDFLAGS         = -module @PLUGIN_LDFLAGS@
unity_la_LDFLAGS            = -module @PLUGIN_LDFLAGS@
webkit_la_LDFLAGS           = -module @PLUGIN_LDFLAGS@
xmppconsole_la_LDFLAGS      = -module @PLUGIN_LDFLAGS@

if PLUGINS

plugin_LTLIBRARIES = \
	extplacement.la     \
	gtkbuddynote.la     \
	history.la          \
	iconaway.la         \
	imgupload.la        \
	markerline.la       \
	notify.la           \
	relnot.la           \
	screencap.la        \
	sendbutton.la       \
	spellchk.la         \
	webkit.la           \
	xmppconsole.la

if ENABLE_UNITY
plugin_LTLIBRARIES +=	unity.la
endif

noinst_LTLIBRARIES = \
	contact_priority.la \
	gtk_signals_test.la

contact_priority_la_SOURCES = contact_priority.c
extplacement_la_SOURCES     = extplacement.c
gtk_signals_test_la_SOURCES = gtk-signals-test.c
gtkbuddynote_la_SOURCES     = gtkbuddynote.c
history_la_SOURCES          = history.c
iconaway_la_SOURCES         = iconaway.c
imgupload_la_SOURCES        = imgupload.c
markerline_la_SOURCES       = markerline.c
notify_la_SOURCES           = notify.c
relnot_la_SOURCES           = relnot.c
screencap_la_SOURCES        = screencap.c
sendbutton_la_SOURCES       = sendbutton.c
spellchk_la_SOURCES         = spellchk.c
unity_la_SOURCES            = unity.c
webkit_la_SOURCES           = webkit.c
xmppconsole_la_SOURCES      = xmppconsole.c

contact_priority_la_LIBADD  = @PIDGIN_LIBS@
extplacement_la_LIBADD      = @PIDGIN_LIBS@
gtk_signals_test_la_LIBADD  = @PIDGIN_LIBS@
gtkbuddynote_la_LIBADD      = @PIDGIN_LIBS@
history_la_LIBADD           = @PIDGIN_LIBS@
iconaway_la_LIBADD          = @PIDGIN_LIBS@
imgupload_la_LIBADD         = @PIDGIN_LIBS@ $(JSON_LIBS)
markerline_la_LIBADD        = @PIDGIN_LIBS@ $(WEBKIT_LIBS)
notify_la_LIBADD            = @PIDGIN_LIBS@
relnot_la_LIBADD            = @PIDGIN_LIBS@
screencap_la_LIBADD         = @PIDGIN_LIBS@
sendbutton_la_LIBADD        = @PIDGIN_LIBS@
spellchk_la_LIBADD          = @PIDGIN_LIBS@
unity_la_LIBADD             = @PIDGIN_LIBS@ $(UNITY_LIBS)
webkit_la_LIBADD            = @PIDGIN_LIBS@ $(WEBKIT_LIBS)
xmppconsole_la_LIBADD       = @PIDGIN_LIBS@

imgupload_la_CFLAGS         = $(JSON_CFLAGS)

endif # PLUGINS

EXTRA_DIST = \
	Makefile.mingw \
	mailchk.c \
	pidgininc.c \
	raw.c \
	win32/transparency/Makefile.mingw \
	win32/transparency/win2ktrans.c \
	win32/winprefs/gtkappbar.c \
	win32/winprefs/gtkappbar.h \
	win32/winprefs/Makefile.mingw \
	win32/winprefs/winprefs.c

AM_CPPFLAGS = \
	-I$(top_builddir)/libpurple \
	-I$(top_srcdir)/libpurple \
	-I$(top_srcdir)/pidgin \
	$(DEBUG_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GPLUGIN_CFLAGS) \
	$(UNITY_CFLAGS) \
	$(WEBKIT_CFLAGS) \
	$(GSTREAMER_CFLAGS) \
	$(PLUGIN_CFLAGS)

#
# This part allows people to build their own plugins in here.
# Yes, it's a mess.
#
SUFFIXES = .c .so
.c.so:
	$(LIBTOOL) --mode=compile $(CC) -DHAVE_CONFIG_H -I$(top_builddir) $(AM_CPPFLAGS) $(CFLAGS) -c $< -o tmp$@.lo $(PLUGIN_CFLAGS)
	$(LIBTOOL) --mode=link    $(CC) $(CFLAGS) -o libtmp$@.la -rpath $(plugindir) tmp$@.lo $(LIBS) $(LDFLAGS) -module @PLUGIN_LDFLAGS@ $(PLUGIN_LIBS)
	@rm -f tmp$@.lo tmp$@.o libtmp$@.la
	@cp .libs/libtmp$@*.so $@
	@rm -rf .libs/libtmp$@.*
