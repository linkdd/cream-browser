include common.mk

all: cream-browser_build.h
	@make -C $(SRCDIR)/ all

clean:
	@make -C $(SRCDIR) clean
	@rm -vrf cream-browser_build.h

install:
	@echo "Installing $(DESTDIR)$(BINDIR)/cream-browser..."
	@install -d $(DESTDIR)$(BINDIR)
	@install -c $(SRCDIR)/$(EXEC) $(DESTDIR)$(BINDIR)/cream-browser -m 755
	
	@echo "Installing $(DESTDIR)$(DATADIR)/cream-browser..."
	@install -d $(DESTDIR)$(DATADIR)/cream-browser/cream/
	@install -c $(SRCDIR)/lua/cream/* $(DESTDIR)$(DATADIR)/cream-browser/cream/ -m 644

uninstall:
	@echo "Uninstalling $(DESTDIR)$(BINDIR)/cream-browser..."
	@rm -rf $(DESTDIR)$(BINDIR)/cream-browser
	@echo "Uninstalling $(DESTDIR)$(DATADIR)/cream-browser..."
	@rm -rf $(DESTDIR)$(DATADIR)/cream-browser

doc:
	@mkdir -p docs/lua docs/cream-browser
	luadoc $(SRCDIR)/lua/**/*.lua -d docs/lua --nofiles
	doxygen Doxyfile

cream-browser_build.h: cream-browser_build.h.in
	@echo "-- Generating cream-browser_build.h ..."
	@sed -e "s:@HAVE_DEBUG@:$(HAVE_DEBUG):"      \
		-e "s:@GLIB_VERSION@:$(GLIB_VERSION):"  \
		-e "s:@GTK_VERSION@:$(GTK_VERSION):"    \
		-e "s:@LUA_VERSION@:$(LUA_VERSION):"    \
		-e "s:@CC@:$(CC):"                      \
		-e "s:@CFLAGS@:$(CFLAGS):"              \
		-e "s:@LDFLAGS@:$(LDFLAGS):"            \
		-e "s:@ARCH@:$(ARCH):"                  \
		-e "s:@PACKAGE@:$(PACKAGE):"            \
		-e "s:@VERSION@:$(VERSION):"            \
		-e "s:@LICENSE@:$(LICENSE):"            \
		-e "s:@PREFIX@:$(PREFIX):"              \
		-e "s:@EXEC_PREFIX@:$(EXEC_PREFIX):"    \
		-e "s:@SYSCONFDIR@:$(SYSCONFDIR):"      \
		-e "s:@BINDIR@:$(BINDIR):"              \
		-e "s:@LIBDIR@:$(LIBDIR):"              \
		-e "s:@DATADIR@:$(DATADIR):"            \
		-e "s:@LOCALEDIR@:$(LOCALEDIR):"        \
		-e "s:@MANDIR@:$(MANDIR):"              \
		-e "s:@INFODIR@:$(INFODIR):" $^ > $@

.PHONY: all clean install uninstall cream-browser_build.h
