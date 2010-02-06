AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/../../core -I$(srcdir)/modules -I$(srcdir)/curl
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcream-curl.a
libcream_curl_a_SOURCES = CurlModule.c
libcream_curl_a_SOURCES += CurlModule.h
libcream_curl_a_CFLAGS = $(GTK_CFLAGS) $(CURL_CFLAGS)
