TOP=.
include ${TOP}/Makefile.config

CFLAGS = $(GTK2_CFLAGS)
LDLIBS = $(GTK2_LIBS)

SUBDIR= src

all: all-subdir

install: install-subdir install-other

deinstall: deinstall-subdir deinstall-other

clean: clean-subdir

cleandir: cleandir-subdir

distclean: clean cleandir
	rm -rf config config.log config_build.h Makefile.config
	rm -rf configure.lua
	rm -rf doc/*
	rm -rf src/marshal.h src/marshal.c

depend: depend-subdir

.PHONY: doc
doc:
	@mkdir -p docs/lua docs/cream-browser
	luadoc src/lua/*.lua -d docs/lua --nofiles
	doxygen Doxyfile

reconfigure:
	rm -f configure
	cat configure.in | mkconfigure > configure
	chmod 755 configure
	./configure

include ${TOP}/mk/build.subdir.mk
