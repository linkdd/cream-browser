#!/bin/sh

DEPENDANCIES="gtk+-2.0 gnet-2.0 webkit-1.0"
VERSIONS="2.14 2.0.0 1.1.15"

HEADERS="confuse.h"
LIBRARIES="libconfuse.so"

echo "Checking programs:"

# Check PKG-CONFIG
echo -n "-- Checking pkg-config... "
PKGCONFIG="`which pkg-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program pkg-config is missing."
     exit 1
fi
echo "yes"

# Check CURL-CONFIG
echo -n "-- Checking curl-config... "
CURLCONFIG="`which curl-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program curl-config is missing."
     exit 1
fi
echo "yes"

echo "Checking libraries (pkg-config):"

# Check LIBRARIES
i=0
for lib in $DEPENDANCIES
do
     ver=""
     j=0

     for ver in $VERSIONS
     do
          if [ $i -eq $j ] ; then
               break
          fi
          j=$((j + 1))
     done

     echo -n "-- Checking $lib... "

     # Library exists ?
     $PKGCONFIG --exists $lib
     if [ "$?" != "0" ] ; then
          echo "no"
          echo "Library $lib is missing."
          exit 1
     fi

     # Is it a good version ?
     $PKGCONFIG --atleast-version=$ver $lib
     if [ "$?" != "0" ] ; then
          echo "no"
          echo "Error: Your version is too old"
          echo "$lib: `$PKGCONFIG --modversion $lib` < $ver"
          exit 1
     fi

     echo "yes"
     i=$((i + 1))
done

echo "Checking libraries:"

# Check libraries without pkg-config
for h in $HEADERS
do
	echo -n "-- Checking $h..."
	if [ ! -e /usr/include/$h ] && [ ! -e /usr/local/include/$h ] ; then
		echo "no"
		echo "Header $h not found."
		exit 1
	fi

	echo "yes"
done

for l in $LIBRARIES
do
	echo -n "-- Checking $l..."
	if [ ! -e /usr/lib/$l ] && [ ! -e /usr/local/lib/$l ] ; then
		echo "no"
		echo "Library $l not found."
		exit 1
	fi

	echo "yes"
done

