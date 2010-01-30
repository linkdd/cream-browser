AC_INIT([cream-browser], [201001], [linkdd@ydb.me])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

AC_CHECK_PROGS([PKGCONFIG], [pkg-config], [:])
if test "$PKGCONFIG" = :; then
    AC_MSG_ERROR([Package 'pkg-config' not found])
fi

AC_PROG_CC
AC_PROG_RANLIB
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/core/Makefile
    src/libs/Makefile
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

AC_OUTPUT
