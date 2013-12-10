#!/bin/bash

echo "Enter the user"
read USER

if [! -d "$USER" ] 
then 
	exit 1
fi


echo "Enter the password"
read PASS


	
