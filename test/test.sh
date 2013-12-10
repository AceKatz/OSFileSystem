# Test script to display functionality of the FUSE filesystem written
#	by Jack Shannon, Brannon McGraw and Eli Katz for CSCI 3412 at
#	GWU for the Fall 2013 semester.
# Script written by Eli Katz

#!/bin/bash


mount()
{
if [ $1 -eq 0 ]
then 
	echo "Mounting file system..."
	if [ -d "../shadow" ] 
	then
		umount -f ../shadow 
		rm -rf ../shadow
	fi

	mkdir ../shadow
	if [ -z "$2" ] && [ "$2" -eq 1 ]
	then 
		../src/shfs ../shadow ../src/shadow.test -f 
	else
		../src/shfs ../shadow ../src/shadow.test 
	fi
	cd ../shadow
elif [ $1 -eq 1 ]
then
	if [ -n "$2" ]
	then
		cd "$2"
	else
		echo "Please enter correct parameters."
		correct_syntax
else
	correct_syntax
fi
}


testy()
{
	echo
	echo "All directories in /:"
	ls
	echo
	echo "Contents of /root:"
	ls root
	echo
	echo "Contents of /sys:"
	ls sys
	echo
	cd /sys
	for i in *
	do	
		echo "Contents of /sys/$i"
		cat $i
	done
	
	echo "Creating new user, named 'test'"
	cd ../
	mkdir "test"
	cd "test"
	ls
	cat "./days_since_changed"
	echo
	echo "Creating new file, extra_file.txt"
	echo "this is a test" > extra_file.txt
	cat extra_file.txt
	echo
	echo "Listing all files in ./test"
	ls
	

}

correct_syntax() 
{
	echo 
	echo
	echo "Usage: test [PARAM1] [PARAM2]"
	echo "Execute or mount the test function for the shadowfile FUSE filesystem written for CS3412."
	echo 
	echo "Required value of parameters varies by the value:"
	echo "PARAM1:"
	awk "{\"%-5s %-30s\n\",\"0\", \"The main code has not yet been run, so this script must execute and mount the filesystem.  It creates and cd's to said directory.  If the second parameter has a value of 1, then it prints the system calls it makes as it executes.  If the second param has any other value, or is null, it does not print the syscalls.\"}"
	awk "{\"%-5s %-30s\n\",\"1\", \"The main code has been executed, and the user must provide a target directory as PARAM2.\n\"}"

}

mount

testy





