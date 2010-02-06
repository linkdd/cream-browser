AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/../../core -I$(srcdir)/.. -I$(srcdir)/../modules -I$(srcdir)/../curl
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcreamview.a
libcreamview_a_SOURCES = CreamView.c CreamBackwardForwardList.c
libcreamview_a_SOURCES += CreamView.h CreamBackwardForwardList.h
libcreamview_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS)
