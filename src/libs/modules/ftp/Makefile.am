AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/include
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcream-module-ftp.a
libcream_module_ftp_a_SOURCES = FTPModule.c
libcream_module_ftp_a_SOURCES += FTPModule.h
libcream_module_ftp_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CLFAGS)
