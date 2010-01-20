AC_INIT([cream-browser], [201001], [linkdd62@gmail.com])
AM_INIT_AUTOMAKE([-Wall foreign])

AM_GNU_GETTEXT_VERSION([0.17])
AM_GNU_GETTEXT([external])

AC_CHECK_PROGS([PKGCONFIG], [pkg-config], [:])
if test "$PKGCONFIG" = :; then
    AC_MSG_ERROR([Package 'pkg-config' not found])
fi

AC_PROG_CC
AC_PROG_RANLIB
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([ po/Makefile.in
    Makefile
    src/Makefile
    src/core/Makefile
    src/libs/Makefile
])

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.14)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(WEBKIT, webkit-1.0 >= 1.1.6)
AC_SUBST(WEBKIT_CFLAGS)
AC_SUBST(WEBKIT_LIBS)

AC_OUTPUT
