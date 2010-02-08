AC_INIT([cream-browser], [201002], [linkdd@ydb.me])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CHECK_PROGS([PKGCONFIG], [pkg-config], [:])
if test "$PKGCONFIG" = :; then
    AC_MSG_ERROR([Package 'pkg-config' not found])
fi

AC_CHECK_PROGS([CURLCONFIG], [curl-config], [:])
if test "$CURLCONFIG" = :; then
     AC_MSG_ERROR([Package 'curl-config' not found])
fi

AC_PROG_CC
AC_PROG_RANLIB
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/core/Makefile
    src/libs/Makefile
    src/libs/curl/Makefile
    src/libs/CreamView/Makefile
    src/libs/modules/about/Makefile
    src/libs/modules/ftp/Makefile
    src/libs/modules/www/Makefile
])

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.14)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.1.18)
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)

AC_SUBST(CURL_CFLAGS, `curl-config --cflags`)
AC_SUBST(CURL_LIBS, `curl-config --libs`)

PKG_CHECK_MODULES(GNET, gnet-2.0 >= 2.0.0)
AC_SUBST(GNET_CFLAGS)
AC_SUBST(GNET_LIBS)

CFLAGS="$CFLAGS $GTK_CFLAGS $WEBKIT_CFLAGS $CURL_CFLAGS $GNET_CFLAGS"
LIBS="$LIBS $GTK_LIBS $WEBKIT_LIBS $CURL_LIBS $GNET_LIBS $LIBINTL"

AC_OUTPUT
