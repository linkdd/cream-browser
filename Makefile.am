SUBDIRS = src/libs src/core

install-data-hook:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(sysconfdir)/xdg/cream-browser/
	$(INSTALL_DATA) $(top_srcdir)/config.sample $(DESTDIR)$(sysconfdir)/xdg/cream-browser/config
