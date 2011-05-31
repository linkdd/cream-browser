include common.mk

project-all: cream-browser_build.h
	@make -C $(SRCDIR)/ all

project-clean:
	@make -C $(SRCDIR) clean

project-install:
	@echo "Installing $(DESTDIR)$(BINDIR)/$(EXEC)..."
	@install -d $(DESTDIR)$(BINDIR)
	@install -c $(SRCDIR)/$(EXEC) $(DESTDIR)$(BINDIR)/$(EXEC) -m 755
	
	@echo "Installing $(DESTDIR)$(DATADIR)/$(EXEC)..."
	@install -d $(DESTDIR)$(DATADIR)/$(EXEC)/cream/
	@install -c $(SRCDIR)/lua/cream/* $(DESTDIR)$(DATADIR)/$(EXEC)/cream/ -m 644
	
	@echo "Installing locale..."
	@for mo in `find po/ -name "*.mo" -printf "%f "`; do \
		install -d $(DESTDIR)$(LOCALEDIR)/$${mo%.mo}/LC_MESSAGES/; \
		install -c po/$$mo $(DESTDIR)$(LOCALEDIR)/$${mo%.mo}/LC_MESSAGES/$(EXEC).mo -m 644; \
	done

project-uninstall:
	@echo "Uninstalling $(DESTDIR)$(BINDIR)/$(EXEC)..."
	@rm -rf $(DESTDIR)$(BINDIR)/$(EXEC)
	@echo "Uninstalling $(DESTDIR)$(DATADIR)/$(EXEC)..."
	@rm -rf $(DESTDIR)$(DATADIR)/$(EXEC)
	@echo "Uninstalling locale..."
	@find $(DESTDIR)$(LOCALEDIR) -name "$(EXEC).mo" -delete

project-maintainer-clean:
	@rm -vrf cream-browser_build.h

.PHONY: doc
doc:
	@mkdir -p docs/lua docs/cream-browser
	luadoc $(SRCDIR)/lua/**/*.lua -d docs/lua --nofiles
	doxygen Doxyfile

.PHONY: cream-browser_build.h
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


