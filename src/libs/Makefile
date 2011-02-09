TOP=	../..
include	${TOP}/Makefile.config

LIB=	cream
SRCS=	errors.c	\
	config.c	\
	modules.c

CFLAGS+=	-I${TOP}/src/include		\
		${GLIB2_CFLAGS}			\
		${GTK2_CFLAGS}			\
		${LUA_CFLAGS}

include	${TOP}/mk/build.lib.mk
