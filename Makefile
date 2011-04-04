include common.mk

all: cream-browser_build.h
	@make -C $(SRCDIR)/ all

clean:
	@make -C $(SRCDIR) clean
	@rm -vrf cream-browser_build.h

install:
	@echo "Installing $(DESTDIR)$(PREFIX)/bin/cream-browser..."
	@install -c $(SRCDIR)/$(EXEC) $(DESTDIR)$(PREFIX)/bin/cream-browser

uninstall:
	@echo "Uninstalling $(DESTDIR)$(PREFIX)/bin/cream-browser..."
	@rm -rf $(DESTDIR)$(PREFIX)/bin/cream-browser

doc:
	@mkdir -p docs/lua docs/cream-browser
	luadoc src/lua/**/*.lua -d docs/lua --nofiles
	doxygen Doxyfile

cream-browser_build.h: cream-browser_build.h.in
	@echo "-- Generating cream-browser_build.h ..."
	@sed -e "s:@HAVE_DEBUG@:$(HAVE_DEBUG):"      \
	     -e "s:@GLIB_VERSION@:$(GLIB_VERSION):"  \
	     -e "s:@GTK_VERSION@:$(GTK_VERSION):"    \
	     -e "s:@LUA_VERSION@:$(LUA_VERSION):"    \
	     -e "s:@CFLAGS@:$(CFLAGS):"              \
	     -e "s:@LDFLAGS@:$(LDFLAGS):"            \
	     -e "s:@ARCH@:$(ARCH):"                  \
	     -e "s:@PACKAGE@:$(PACKAGE):"            \
	     -e "s:@VERSION@:$(VERSION):"            \
	     -e "s:@MODULES@:$(MODULES):"            \
	     -e "s:@PREFIX@:$(PREFIX):"              \
	     -e "s:@SYSCONFDIR@:$(SYSCONFDIR):" $^ > $@

modules:
	@$(SRCDIR)/modules/build-modules.sh $(MODULES)

.PHONY: all clean install uninstall cream-browser_build.h
