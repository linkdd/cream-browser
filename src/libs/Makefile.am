noinst_LIBRARIES = libcream.a
libcream_a_SOURCES = CreamView/CreamView.c CreamView/CreamBackwardForwardList.c \
                     config/libconfig.c \
                     curl/CurlClient.c \
                     gopher/GopherClient.c \
                     modules/about/AboutModule.c \
                     modules/ftp/FTPModule.c \
                     modules/www/WebViewModule.c modules/www/Favicon.c \
                     CreamView/CreamView.h CreamView/CreamBackwardForwardList.h \
                     config/libconfig.h \
                     curl/CurlClient.h \
                     gopher/GopherClient.h \
                     modules/about/AboutModule.h \
                     modules/ftp/FTPModule.h \
                     modules/www/WebViewModule.h modules/www/Favicon.h

libcream_a_CPPFLAGS = $(GTK_CFLAGS) $(GNET_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS) \
                       -I$(top_srcdir) -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/config -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher -I$(top_srcdir)/src/libs/modules \
                       -DPREFIX="\"$(prefix)\""

