TOP=.
include ${TOP}/Makefile.config

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

depend: depend-subdir

.PHONY: doc
doc:
	luadoc src/lua/*.lua -d doc --nofiles

reconfigure:
	rm -f configure
	cat configure.in | mkconfigure > configure
	chmod 755 configure
	./configure

include ${TOP}/mk/build.subdir.mk
