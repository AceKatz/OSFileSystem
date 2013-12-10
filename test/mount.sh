# Test script to display functionality of the FUSE filesystem written
#	by Jack Shannon, Brannon McGraw and Eli Katz for CSCI 3412 at
#	GWU for the Fall 2013 semester.
# Script written by Eli Katz

#!/bin/bash


mount()
{
	if [ -d "../shadow" ] 
	then
		rm -rf ../shadow
	fi
	mkdir ../shadow
	../src/shfs ../shadow ../src/shadow.test -f
	cd ../shadow
}

mount
