AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/modules
DEFS = -DPREFIX=\"$(prefix)\"

SUBDIRS = modules/about modules/ftp modules/www

lib_LIBRARIES = libcream.a
libcream_a_SOURCES = CreamView.c
libcream_a_SOURCES += CreamView.h
libcream_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
