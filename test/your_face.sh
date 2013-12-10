
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
	echo "Creating new file, extra_file.txt"
	echo "42" > days_since_changed
	cat extra_file.txt
	echo
	echo "Listing all files in ./test"
	ls
	

}
testy
