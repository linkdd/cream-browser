AC_INIT([cream-browser], [201003], [linkdd@ydb.me])
AC_CONFIG_AUX_DIR([support])
AM_INIT_AUTOMAKE([-Wall foreign])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

AC_CHECK_PROGS([CURLCONFIG], [curl-config], [:])
if test "$CURLCONFIG" = :; then
     AC_MSG_ERROR([curl-config: Not found])
fi

# Checks for libraries

PKG_CHECK_MODULES(GTK, gtk+-2.0)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(GNET, gnet-2.0)
AC_SUBST(GNET_CFLAGS)
AC_SUBST(GNET_LIBS)

PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.1.15)
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)

AC_SUBST(CURL_CFLAGS, [`curl-config --cflags`])
AC_SUBST(CURL_LIBS, [`curl-config --libs`])

# Checks for library functions

AC_CHECK_LIB([confuse], [cfg_init],    [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_init() missing])])
AC_CHECK_LIB([confuse], [cfg_parse],   [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_parse() missing])])
AC_CHECK_LIB([confuse], [cfg_getsec],  [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_getsec() missing])])
AC_CHECK_LIB([confuse], [cfg_getnsec], [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_getnsec() missing])])
AC_CHECK_LIB([confuse], [cfg_getstr],  [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_getstr() missing])])
AC_CHECK_LIB([confuse], [cfg_getbool], [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_getbool() missing])])
AC_CHECK_LIB([confuse], [cfg_title],   [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_title() missing])])
AC_CHECK_LIB([confuse], [cfg_size],    [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_size() missing])])
AC_CHECK_LIB([confuse], [cfg_free],    [CONFUSE_LIBS=-lconfuse], [AC_MSG_ERROR([libconfuse: cfg_free() missing])])
AC_SUBST(CONFUSE_LIBS)

# Checks for header files

AC_CHECK_HEADERS(confuse.h)

# Checks for library features

if test "`curl-config --feature | grep SSL`" == "SSL"; then
     AC_DEFINE([HAVE_CURL_SSL], [1], Have curl SSL support ?[])
fi

# Output files

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
     Makefile
     src/core/Makefile
     src/libs/Makefile
])

AC_OUTPUT
