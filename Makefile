include common.mk

all:
	@make -C $(SRCDIR)/ all

clean:
	@make -C $(SRCDIR) clean

install:
	@echo "Installing $(DESTDIR)$(PREFIX)/bin/cream-browser..."
	@install -c $(SRCDIR)/$(EXEC) $(DESTDIR)$(PREFIX)/bin/cream-browser

uninstall:
	@echo "Uninstalling $(DESTDIR)$(PREFIX)/bin/cream-browser..."
	@rm -rf $(DESTDIR)$(PREFIX)/bin/cream-browser

doc:
	@mkdir -p docs/lua docs/cream-browser
#	luadoc src/lua/*.lua -d docs/lua --nofiles
	doxygen Doxyfile

.PHONY: all clean install uninstall
