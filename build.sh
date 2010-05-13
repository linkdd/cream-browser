#!/bin/sh

ENABLE_VTE="yes"

print_help()
{
     echo "Usage: $0 [options...]"
     echo ""
     echo "Options:"
     echo "     --prefix=<PREFIX>            Use PREFIX for installation (default: /usr)"
     echo "     --sysconfdir=<SYSCONFDIR>    Use SYSCONFDIR for configuration installation (default: ${PREFIX}/etc"
     echo "     --enable-vte                 Enable VTE Terminal support (if present)"
     echo "     --disable-vte                Disable VTE Terminal support (even if present)"
     echo "     --help                       Print this message"
}

for i in $*
do
     case $i in
          --prefix=*)
               PREFIX="${i##*=}"
               ;;

          --sysconfdir=*)
               SYSCONFDIR="${i##*=}"
               ;;

          --enable-vte)
               ENABLE_VTE="yes"
               ;;

          --disable-vte)
               ENABLE_VTE="no"
               ;;

          --help)
               print_help
               exit 0
               ;;

          *)
               echo "Unknow argument: $i"
               exit 1
               ;;
     esac
done

rm -rf __build__
mkdir __build__
cd __build__

# Configure
OPTIONS="-D USE_VTE=${ENABLE_VTE}"

if [ "x${PREFIX}" != "x" ] ; then
     OPTIONS="${OPTIONS} -D PREFIX=${PREFIX}"
fi

if [ "x${SYSCONFDIR}" != "x" ] ; then
     OPTIONS="${OPTIONS} -D SYSCONFDIR=${SYSCONFDIR}"
fi

cmake ${OPTIONS} ..

# Build
make
