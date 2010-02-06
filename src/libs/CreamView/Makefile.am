AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/modules -I$(top_srcdir)/src/libs/curl
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcreamview.a
libcreamview_a_SOURCES = CreamView.c CreamBackwardForwardList.c
libcreamview_a_SOURCES += CreamView.h CreamBackwardForwardList.h
libcreamview_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS)
