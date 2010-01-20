AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/../include
DEFS = -DPREFIX=\"$(prefix)\"

bin_PROGRAMS = cream
cream_SOURCES = main.c local.h structs.h interface.c interface.h callbacks.c callbacks.h
cream_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
cream_LDADD = $(GTK_LIBS) $(WEBKIT_LIBS) $(LIBINTL) ../libs/libcream.a
