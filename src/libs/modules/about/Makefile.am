AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/include
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcream-module-about.a
libcream_module_about_a_SOURCES = AboutModule.c
libcream_module_about_a_SOURCES += AboutModule.h
libcream_module_about_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
