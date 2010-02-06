AM_CPPFLAGS = -I..//libs -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/modules -I$(top_srcdir)/src/libs/curl
DEFS = -DPREFIX=\"$(prefix)\"

bin_PROGRAMS = cream creamctl
cream_SOURCES = main.c local.h structs.h interface.c interface.h callbacks.c callbacks.h CreamTabbed.c CreamTabbed.h marshal.c marshal.h
cream_CFLAGS = $(GTK_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS)
cream_LDADD = $(GTK_LIBS) $(WEBKIT_LIBS) $(CURL_LIBS) $(LIBINTL) \
              ../libs/CreamView/libcreamview.a \
              ../libs/curl/libcream-curl.a \
              ../libs/modules/about/libcream-module-about.a \
              ../libs/modules/ftp/libcream-module-ftp.a \
              ../libs/modules/www/libcream-module-webview.a

creamctl_SOURCES = creamctl.c
creamctl_CFLAGS = $(GTK_CFLAGS)
creamctl_LDADD = $(GTK_LIBS) $(LIBINTL)
