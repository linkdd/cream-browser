include common.mk

SOURCES += mod-dummy.c
HEADERS += mod-dummy.h
OBJECTS = $(SOURCES:.c=.o)

module-all: $(LIBNAME)

module-clean:
	@rm -vrf $(OBJECTS) $(LIBNAME)

module-install:
	@echo "Installing $(DESTDIR)$(LIBDIR)/$(LIBNAME)..."
	@install -d $(DESTDIR)$(LIBDIR)
	@install -c $(LIBNAME) $(DESTDIR)$(LIBDIR)/$(LIBNAME) -m 755

module-uninstall:
	@echo "Uninstalling $(DESTDIR)$(LIBDIR)/$(LIBNAME)..."
	@rm -rf $(DESTDIR)$(LIBDIR)/$(LIBNAME)

module-maintainer-clean:


$(LIBNAME): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

