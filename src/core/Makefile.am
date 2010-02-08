AM_CPPFLAGS = -I..//libs -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/modules -I$(top_srcdir)/src/libs/curl
DEFS = -DPREFIX=\"$(prefix)\"

bin_PROGRAMS = cream creamctl
cream_SOURCES = main.c marshal.c interface.c callbacks.c CreamTabbed.c command.c
cream_SOURCES += local.h structs.h marshal.h interface.h callbacks.h CreamTabbed.h command.h
cream_LDADD = ../libs/CreamView/libcreamview.a \
              ../libs/curl/libcream-curl.a \
              ../libs/modules/about/libcream-module-about.a \
              ../libs/modules/ftp/libcream-module-ftp.a \
              ../libs/modules/www/libcream-module-webview.a

creamctl_SOURCES = creamctl.c
