#!/bin/sh
# vim:set et sts=4 ts=4 tw=80:
echo '==> rm -r _build'
rm -r _build

echo '==> mkdir _build'
mkdir _build
if [ "$?" != "0" ]; then
    exit 1
fi

echo '==> cd _build'
cd _build
if [ "$?" != "0" ]; then
    exit 1
fi

echo '==> cmake ..'
cmake ..
if [ "$?" != "0" ]; then
    exit 1
fi

echo '==> make'
make
if [ "$?" != "0" ]; then
    exit 1
fi

echo '==> leaving _build'
echo "==> You can now \`cd _build'"
echo "==> and type \`sudo make install' to install CreamBrowser"
