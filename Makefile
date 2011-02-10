TOP=.
include ${TOP}/Makefile.config

SUBDIR= src

all: all-subdir

install: install-subdir install-other

deinstall: deinstall-subdir deinstall-other

clean: clean-subdir

cleandir: cleandir-subdir

distclean: clean cleandir
	rm -rf mk/libtool*
	rm -rf config config.log config_build.h Makefile.config
	rm -rf configure.lua
	rm -rf src/core/.depend src/libs/.depend
	rm -rf src/core/.libs src/libs/.libs

depend: depend-subdir

reconfigure:
	rm -f configure
	cat configure.in | mkconfigure > configure
	chmod 755 configure
	./configure

include ${TOP}/mk/build.subdir.mk
