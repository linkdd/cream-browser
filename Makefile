include common.mk

all:
	@make -C $(SRCDIR)/libs all
	@make -C $(SRCDIR)/core all

modules: all
	@make -C $(SRCDIR)/libs/modules $(MODULES)

clean:
	@$(LIBTOOL) --mode=clean rm -f $(SRCDIR)/**/*.lo
	@$(LIBTOOL) --mode=clean rm -f $(SRCDIR)/**/*.la
	@$(LIBTOOL) --mode=clean rm -f $(SRCDIR)/core/cream-browser

install:
	@$(LIBTOOL) --mode=install install -c $(SRCDIR)/core/cream-browser $(DESTDIR)$(PREFIX)/bin/cream-browser
	@$(LIBTOOL) --mode=install install -c $(SRCDIR)/libs/libcream.la $(DESTDIR)$(PREFIX)/lib/libcream.la
	@$(LIBTOOL) --finish $(DESTDIR)$(PREFIX)/lib

uninstall:
	@$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(PREFIX)/lib/libcream.la
	@$(LIBTOOL) --mode=uninstall rm -f $(DESTDIR)$(PREFIX)/bin/cream-browser

.PHONY: all clean install uninstall
