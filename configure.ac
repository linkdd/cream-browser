AC_INIT([cream-browser], [201004], [linkdd@ydb.me])
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

AC_CHECK_LIB([confuse], [main], [], [
echo "Library 'libconfuse' not found"
exit -1
])
AC_SUBST(CONFUSE_LIBS, [-lconfuse])

# Checks for library functions

# Checks for header files

# Checks for library features

if test "`curl-config --feature | grep SSL`" == "SSL"; then
     AC_DEFINE([HAVE_CURL_SSL], [1], [Have curl SSL support ?])
fi

# Output files

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
     Makefile
     src/core/Makefile
     src/libs/Makefile
])

arch=`uname -m`
uname_a=`uname -a`
uname_s=`uname -s`
uname_n=`uname -n`
uname_r=`uname -r`
uname_v=`uname -v`
uname_p=`uname -p`
uname_i=`uname -i`
uname_o=`uname -o`

AC_DEFINE_UNQUOTED([ARCH],    "${arch}",    [Architecture])
AC_DEFINE_UNQUOTED([UNAME_A], "${uname_a}", [All informations])
AC_DEFINE_UNQUOTED([UNAME_S], "${uname_s}", [Kernel name])
AC_DEFINE_UNQUOTED([UNAME_N], "${uname_n}", [Hostname])
AC_DEFINE_UNQUOTED([UNAME_R], "${uname_r}", [Kernel release])
AC_DEFINE_UNQUOTED([UNAME_V], "${uname_v}", [Kernel version])
AC_DEFINE_UNQUOTED([UNAME_P], "${uname_p}", [Processor type])
AC_DEFINE_UNQUOTED([UNAME_I], "${uname_i}", [Hardware platform])
AC_DEFINE_UNQUOTED([UNAME_O], "${uname_o}", [Openrating System])

AC_OUTPUT
