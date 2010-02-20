PROGNAME  = cream-browser
VERSION   = 201002
BUGREPORT = linkdd@ydb.me

# Variables for 'make dist'
CREAM_MAKEFILES =	Makefile \
				src/core/Makefile \
				src/libs/Makefile \
				src/libs/CreamView/Makefile \
				src/libs/curl/Makefile \
				src/libs/gopher/Makefile \
				src/libs/modules/about/Makefile \
				src/libs/modules/ftp/Makefile \
				src/libs/modules/www/Makefile

CREAM_DISTFILES =	$(CREAM_MAKEFILES) COPYING AUTHORS README \
				src/core/*.c src/core/*.h \
				src/libs/CreamView/*.c src/libs/CreamView/*.h \
				src/libs/curl/*.c src/libs/curl/*.h \
				src/libs/gopher/*.c src/libs/gopher/*.h \
				src/libs/modules/about/*.c src/libs/modules/about/*.h \
				src/libs/modules/ftp/*.c src/libs/modules/ftp/*.h \
				src/libs/modules/www/*.c src/libs/modules/www/*.h \
				doc/Doxyfile check-dep.sh

# Programs
export INSTALL_PROGRAM = install -c
export INSTALL_SCRIPT  = install -c
export INSTALL_DATA    = install -c -m 644

# Common PREFIX for installation directories.
# NOTE: This directory must exist when you start the install.
ifndef DESTDIR
	export PREFIX = /usr
endif
export datarootdir = $(PREFIX)/share
export datadir = $(datarootdir)
export exec_prefix = $(PREFIX)
# Where to put the executable for the command `gcc'.
export bindir = $(exec_prefix)/bin
# Where to put the directories used by the compiler.
export libexecdir = $(exec_prefix)/libexec
# Where to put the Info files.
export infodir = $(datarootdir)/info
# Sources directory
export top_srcdir = $(PWD)

# GCC flags
INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher -I$(top_srcdir)/src/libs/modules
export CFLAGS += -g -ggdb3 -fno-inline -Wall -O2 -DPREFIX="$(PREFIX)" $(INCLUDES)

# GTK+ flags for GCC
export GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
export GTK_LDADD = `pkg-config --libs gtk+-2.0`
# GNet flags for GCC
export GNET_CFLAGS = `pkg-config --cflags gnet-2.0`
export GNET_LDADD = `pkg-config --libs gnet-2.0`
# WebKit flags for GCC
export WEBKIT_CFLAGS = `pkg-config --cflags webkit-1.0`
export WEBKIT_LDADD = `pkg-config --libs webkit-1.0`
# Curl flags for GCC
export CURL_CFLAGS = `curl-config --cflags`
export CURL_LDADD = `curl-config --libs`
# All flags for GCC
export ALL_CFLAGS = $(GTK_CFLAGS) $(GNET_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS)
export ALL_LDADD = $(GTK_LDADD) $(GNET_LDADD) $(WEBKIT_LDADD) $(CURL_LDADD)

.PHONY: all install uninstall clean dist doc check-dep

all: check-dep
	@$(MAKE) all -C src/libs
	@$(MAKE) all -C src/core

check-dep:
	@sh check-dep.sh
	@echo "#ifndef __CONFIG_H" > config.h
	@echo "#define __CONFIG_H" >> config.h

	@echo "#define PACKAGE    \"$(PROGNAME)\"" >> config.h
	@echo "#define VERSION    \"$(VERSION)\"" >> config.h
	@echo "#define BUGREPORT  \"$(BUGREPORT)\"" >> config.h

	@if [ "`curl-config --feature | grep SSL`" == "SSL" ]; then echo "#define HAVE_CURL_SSL" >> config.h ; fi
	@echo "#endif /* __CONFIG_H */" >> config.h

install:
	@mkdir -p $(DESTDIR)$(bindir)
	@mkdir -p $(DESTDIR)$(datadir)
	@mkdir -p $(DESTDIR)$(infodir)
	@$(MAKE) install -C src/core DESTDIR=$(DESTDIR)

uninstall:
	@$(MAKE) uninstall -C src/core DESTDIR=$(DESTDIR)

clean:
	@$(MAKE) clean -C src/libs
	@$(MAKE) clean -C src/core
	@rm -vrf config.h

dist:
	@echo "Copying files..."
	@mkdir $(PROGNAME)-$(VERSION)
	@cp --parents $(CREAM_DISTFILES) $(PROGNAME)-$(VERSION)/
	@echo "Creating $(PROGNAME)-$(VERSION).tar ..."
	@tar cvf $(PROGNAME)-$(VERSION).tar $(PROGNAME)-$(VERSION)/* -H posix > /dev/null
	@echo "Creating $(PROGNAME)-$(VERSION).tar.gz ..."
	@gzip $(PROGNAME)-$(VERSION).tar
	@echo "Cleaning..."
	@rm -rf $(PROGNAME)-$(VERSION)

doc:
	@$(MAKE) -C doc/
