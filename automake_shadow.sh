#!/bin/bash

echo "About to run aclocal"
aclocal

echo "About to run autoheader"
autoheader

echo "About to run autoscan"
autoscan
echo "About to run autoconf"

autoconf

echo "About to run libtoolize"
libtoolize

echo "About to run automake --add-missing"
automake --add-missing

echo "About to run touch NEWS README AUTHORS ChangeLog"
touch NEWS README AUTHORS ChangeLog

echo "About to run autoreconf -fi"
autoreconf -fi

echo "About to run automake"
automake

echo "About to run ./configure"
./configure

echo "About to run make clean"
make clean

echo "About to run make"
make
