
testy()
{
	pwd
	cd ../shadow
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
	cd sys
	for i in *
	do	
		echo "Contents of " && pwd
		cat $i
	done
	
	echo "Creating new user, named 'test'"
	cd ..
	mkdir "test"
	cd "test"
	ls
	cat "./days_since_changed"
	echo
	echo "Editing days_since_changed
	echo "42" > days_since_changed"
	cat days_since_changed
	echo
	echo "Listing all files in ./test"
	ls
	echo "Creating new password for test..."
	echo "fuckthisshit" > password-hash
	echo "Displaying new password"
	cat password-hash
 
	

}
testy
