TOP=	../..
include	${TOP}/Makefile.config

PROG=		cream
PROG_TYPE=	GUI

SRCS=		main.c

LIBS=		${TOP}/src/libs/.libs/libcream.a

CFLAGS+=	${GLIB2_CFLAGS}		\
		${GTK2_CFLAGS}		\
		${WEBKIT_CFLAGS}	\
		${LUA_CFLAGS}		\
		-I${TOP}/src/include

include	${TOP}/mk/build.prog.mk
