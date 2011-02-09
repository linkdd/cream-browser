TOP=	.
include	${TOP}/Makefile.config

SUBDIR=	src

all:		all-subdir
install:	install-subdir	install-other
deinstall:	deinstall-subdir	deinstall-other
clean:		clean-subdir
cleandir:	cleandir-subdir
depend:		depend-subdir

reconfigure:
	rm -f configure
	cat configure.in | mkconfigure > configure
	chmod 755 configure

include	${TOP}/mk/build.subdir.mk
