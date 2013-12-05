#!/bin/bash
if [ -d $1 ] 
then 
	cd $1
else
	echo "The target directory does not exist."
	exit 0
fi

aclocal

autoheader

autoconf

automake --add-missing

touch NEWS README AUTHORS ChangeLog

automake --add-missing

./configure

make all
