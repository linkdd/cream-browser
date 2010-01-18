AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\"
DEFS = -DPREFIX=\"$(prefix)\"

bin_PROGRAMS = cream
cream_SOURCES = main.c interface.c callbacks.c
cream_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
cream_LDADD = $(GTK_LIBS) $(WEBKIT_LIBS) $(LIBINTL)
