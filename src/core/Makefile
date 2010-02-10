cream_SOURCES = main.c marshal.c CreamTabbed.c interface.c callbacks.c command.c
cream_OBJECTS = $(cream_SOURCES:.c=.o)
cream_HEADERS = local.h structs.h marshal.h CreamTabbed.h interface.h callbacks.h command.h
cream_LIBS =	$(top_srcdir)/src/libs/CreamView/libcreamview.a \
			$(top_srcdir)/src/libs/curl/libcream-curl.a \
			$(top_srcdir)/src/libs/gopher/libcream-gopher.a \
			$(top_srcdir)/src/libs/modules/about/libcream-module-about.a \
			$(top_srcdir)/src/libs/modules/ftp/libcream-module-ftp.a \
			$(top_srcdir)/src/libs/modules/www/libcream-module-webview.a

creamctl_SOURCES = creamctl.c
creamctl_OBJECTS = $(creamctl_SOURCES:.c=.o)
creamctl_HEADERS =
creamctl_LIBS =

.PHONY: all install uninstall clean

all: cream creamctl


install: all
	$(INSTALL_PROGRAM) cream $(destdir)$(bindir)/cream
	$(INSTALL_PROGRAM) creamctl $(destdir)$(bindir)/creamctl

uninstall:
	rm $(DESTDIR)$(bindir)/cream
	rm $(DESTDIR)$(bindir)/creamctl

clean:
	@rm -vrf *.o cream creamctl

cream: $(cream_OBJECTS) $(cream_HEADERS)
	$(CC) $(CCFLAGS) $(ALL_LIBS) $(cream_OBJECTS) $(cream_LIBS) -o $@

creamctl: $(creamctl_OBJECTS) $(creamctl_HEADERS)
	$(CC) $(CCFLAGS) $(GTK_LIBS) $(creamctl_OBJECTS) $(creamctl_LIBS) -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $(INCLUDES) $(ALL_CFLAGS) -c $^ -o $@
