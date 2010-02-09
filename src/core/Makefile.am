AM_CPPFLAGS = -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/modules -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher
DEFS = -DPREFIX=\"$(prefix)\"

bin_PROGRAMS = cream creamctl
cream_SOURCES = main.c marshal.c interface.c callbacks.c CreamTabbed.c command.c ../libs/curl/CurlClient.c
cream_SOURCES += local.h structs.h marshal.h interface.h callbacks.h CreamTabbed.h command.h
cream_LDADD = ../libs/CreamView/libcreamview.a \
              ../libs/curl/libcream-curl.a \
              ../libs/gopher/libcream-gopher.a \
              ../libs/modules/about/libcream-module-about.a \
              ../libs/modules/ftp/libcream-module-ftp.a \
              ../libs/modules/www/libcream-module-webview.a

creamctl_SOURCES = creamctl.c
