AM_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" -I$(srcdir)/include
DEFS = -DPREFIX=\"$(prefix)\"

lib_LIBRARIES = libcream-module-webview.a
libcream_module_webview_a_SOURCES = WebViewModule.c Favicon.c
libcream_module_webview_a_SOURCES += WebViewModule.h Favicon.h
libcream_module_webview_a_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS)
