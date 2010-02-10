#!/bin/sh

DEPENDANCIES="gtk+-2.0 gnet-2.0 webkit-1.0"

echo -n "Checking pkg-config... "
PKGCONFIG="`which pkg-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program pkg-config is missing."
     exit 1
fi
echo "yes"

echo -n "Checking curl-config... "
CURLCONFIG="`which curl-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program curl-config is missing."
     exit 1
fi
echo "yes"

for lib in $DEPENDANCIES
do
     echo -n "Checking $lib... "
     $PKGCONFIG --exists $lib
     if [ "$?" != "0" ] ; then
          echo "no"
          echo "Library $lib is missing."
          exit 1
     fi
     echo "yes"
done
