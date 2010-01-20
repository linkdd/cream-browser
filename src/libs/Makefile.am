AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/include
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcream.a
libcream_a_SOURCES = CreamView.c modules/about/AboutModule.c modules/ftp/FTPModule.c modules/www/WebViewModule.c modules/www/Favicon.c
libcream_a_SOURCES += CreamView.h modules/about/AboutModule.h modules/ftp/FTPModule.h modules/www/WebViewModule.h modules/www/Favicon.h
libcream_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
