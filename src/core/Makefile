cream_SOURCES = main.c marshal.c CreamTabbed.c interface.c callbacks.c command.c
cream_OBJECTS = $(cream_SOURCES:.c=.o)
cream_HEADERS = local.h structs.h marshal.h CreamTabbed.h interface.h callbacks.h command.h
cream_LDADD =	-L$(top_srcdir)/src/libs/ -lcream

creamctl_SOURCES = creamctl.c
creamctl_OBJECTS = $(creamctl_SOURCES:.c=.o)
creamctl_HEADERS =
creamctl_LDADD =

.PHONY: all install uninstall clean

all: cream creamctl


install: all
	$(INSTALL_PROGRAM) cream $(DESTDIR)$(bindir)/cream
	$(INSTALL_PROGRAM) creamctl $(DESTDIR)$(bindir)/creamctl

uninstall:
	rm $(DESTDIR)$(bindir)/cream
	rm $(DESTDIR)$(bindir)/creamctl

clean:
	@rm -vrf *.o cream creamctl

cream: $(cream_OBJECTS) $(cream_HEADERS)
	$(CC) $(CFLAGS) $(ALL_LDADD) $(cream_OBJECTS) $(cream_LDADD) -o $@

creamctl: $(creamctl_OBJECTS) $(creamctl_HEADERS)
	$(CC) $(CFLAGS) $(GTK_LDADD) $(creamctl_OBJECTS) $(creamctl_LDADD) -o $@

%.o: %.c
	$(CC) $(CFLAGS) $(ALL_CFLAGS) -c $^ -o $@
