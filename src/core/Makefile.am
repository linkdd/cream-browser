bin_PROGRAMS = cream creamctl
cream_SOURCES = main.c marshal.c CreamTabbed.c interface.c callbacks.c command.c bind.c handler.c \
                local.h structs.h marshal.h CreamTabbed.h interface.h callbacks.h command.h bind.h xdg.h

cream_CPPFLAGS = $(GTK_CFLAGS) $(GNET_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS) $(CONFUSE_CFLAGS) \
                 -I$(top_srcdir) -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/config -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher -I$(top_srcdir)/src/libs/modules \
                 -DPREFIX="\"$(prefix)\""

cream_LDADD = $(CONFUSE_LIBS) $(GTK_LIBS) $(GNET_LIBS) $(WEBKIT_LIBS) $(CURL_LIBS) -L../libs/ -lcream

creamctl_SOURCES = creamctl.c
creamctl_CPPFLAGS = $(GTK_CFLAGS) \
                    -I$(top_srcdir) -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/config -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher -I$(top_srcdir)/src/libs/modules \
                    -DPREFIX="\"$(prefix)\""

creamctl_LDADD = $(GTK_LIBS)
