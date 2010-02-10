# Common prefix for installation directories.
# NOTE: This directory must exist when you start the install.
export prefix = /usr/local
export datarootdir = $(prefix)/share
export datadir = $(datarootdir)
export exec_prefix = $(prefix)
# Where to put the executable for the command `gcc'.
export bindir = $(exec_prefix)/bin
# Where to put the directories used by the compiler.
export libexecdir = $(exec_prefix)/libexec
# Where to put the Info files.
export infodir = $(datarootdir)/info
# Sources directory
export top_srcdir = $(PWD)

# GCC flags
export CC = gcc
export INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/src/core -I$(top_srcdir)/src/libs -I$(top_srcdir)/src/libs/CreamView -I$(top_srcdir)/src/libs/curl -I$(top_srcdir)/src/libs/gopher -I$(top_srcdir)/src/libs/modules
export CCFLAGS = -g -Wall -O2
# GTK+ flags for GCC
export GTK_CFLAGS = `pkg-config --cflags gtk+-2.0`
export GTK_LIBS = `pkg-config --libs gtk+-2.0`
# GNet flags for GCC
export GNET_CFLAGS = `pkg-config --cflags gnet-2.0`
export GNET_LIBS = `pkg-config --libs gnet-2.0`
# WebKit flags for GCC
export WEBKIT_CFLAGS = `pkg-config --cflags webkit-1.0`
export WEBKIT_LIBS = `pkg-config --libs webkit-1.0`
# Curl flags for GCC
export CURL_CFLAGS = `curl-config --cflags`
export CURL_LIBS = `curl-config --libs`
# All flags for GCC
export ALL_CFLAGS = $(GTK_CFLAGS) $(GNET_CFLAGS) $(WEBKIT_CFLAGS) $(CURL_CFLAGS)
export ALL_LIBS = $(GTK_LIBS) $(GNET_LIBS) $(WEBKIT_LIBS) $(CURL_LIBS)

.PHONY: all install uninstall clean dist doc check-dep

all: check-dep
	@$(MAKE) all -C src/libs
	@$(MAKE) all -C src/core

check-dep:
	@sh check-dep.sh

install:
	@$(MAKE) install -C src/core DESTDIR=$(DESTDIR)

uninstall:
	@$(MAKE) uninstall -C src/core DESTDIR=$(DESTDIR)

clean:
	@$(MAKE) clean -C src/libs
	@$(MAKE) clean -C src/core

dist:
	@echo "TODO"

doc:
	@$(MAKE) -C doc/
