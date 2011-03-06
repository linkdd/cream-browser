TOP=..
include ${TOP}/Makefile.config

PROG=cream-browser
PROG_TYPE=GUI

SRCS=errors.c modules.c lua-capi.c lua.c config.c socket.c xdg.c main.c

LIBS+=${GLIB2_LIBS} ${GTK2_LIBS} ${WEBKIT_LIBS} ${LUA_LIBS}
CFLAGS+=-I${TOP} ${GLIB2_CFLAGS} ${GTK2_CFLAGS} ${WEBKIT_CFLAGS} ${LUA_CFLAGS}

include ${TOP}/mk/build.prog.mk
