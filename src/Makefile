TOP=..
include ${TOP}/Makefile.config

PROG=cream-browser
PROG_TYPE=GUI

SRCS=marshal.c errors.c modules.c modules/scheme.c WebViewClass.c lua-capi.c lua.c config.c socket.c xdg.c main.c

LIBS+=${GLIB2_LIBS} ${GTK2_LIBS} ${WEBKIT_LIBS} ${LUA_LIBS}
CFLAGS+=-I${TOP} ${GLIB2_CFLAGS} ${GTK2_CFLAGS} ${WEBKIT_CFLAGS} ${LUA_CFLAGS}

include ${TOP}/mk/build.prog.mk

marshal.h: marshal.txt
	glib-genmarshal --header --prefix=cream_marshal marshal.txt > $@

marshal.c: marshal.h marshal.txt
	glib-genmarshal --body --prefix=cream_marshal marshal.txt > $@


