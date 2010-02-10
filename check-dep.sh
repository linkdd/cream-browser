#!/bin/sh

DEPENDANCIES=( gtk+-2.0 gnet-2.0 webkit-1.0 )
VERSIONS=( 2.14 2.0.0 1.1.18 )

# Check PKG-CONFIG
echo -n "Checking pkg-config... "
PKGCONFIG="`which pkg-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program pkg-config is missing."
     exit 1
fi
echo "yes"

# Check CURL-CONFIG
echo -n "Checking curl-config... "
CURLCONFIG="`which curl-config`"
if [ "$?" != "0" ] ; then
     echo "no"
     echo "The program curl-config is missing."
     exit 1
fi
echo "yes"

# Check LIBRARIES
nblibs=${#DEPENDANCIES[@]}
index=0

while [ "$index" -lt "$nblibs" ]
do
     lib=${DEPENDANCIES[$index]}
     ver=${VERSIONS[$index]}

     echo -n "Checking $lib... "

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

     let "index = $index + 1"
done
