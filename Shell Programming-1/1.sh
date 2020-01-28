#!/bin/bash
var=$* #Taking the input from user
if [ -z "$var" ] #Check if it is empty or not. If it is empty print the error message the screen.
then
 	echo "Please enter a input."
else
	varToLower=$(echo "$var" | awk '{print tolower($0)}') #Make the input lowercase
	len=${#var}
	i=0
	mid=$(($len/2)) #Take the length of the string and divide with 2.
	while [ $i -lt $mid ] #if the half of them is same. It should be palindrome string.
	  do
	    if [ "${varToLower:$i:1}" != "${varToLower: -$(($i+1)):1}" ]
	      then
	      echo "$var is not a palindrome"
	      exit
	    fi
	    i=$(($i+1))
	done
	echo "$var is a palindrome"
fi
