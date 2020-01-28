#!/bin/bash

# Author: Ertuğrul Sağdıç <150116061>
# Author: Eray Ayaz <150116053>
# 
# Make this file executable:
# chmod +x myprog.sh
#
# Run as
# ./myprog.sh
# follow instructions

#print the menu interface
BASEDIR=$(pwd)
PS3='Please select an option: '
options=("Is word palindrome?" "Move .c files" "Create a hollowed square" "Find the words" "Delete the empty files"  "Exit")
select opt in "${options[@]}" #take option and evaluate
do
    case $opt in
        "Is word palindrome?")
		chmod +x 1.sh
		read -p "Enter a word: " palindromeWord
		./1.sh $palindromeWord
		;;
        "Move .c files")
		chmod +x 2.sh
		read -p "Enter a pathname(optional): " pathName
		if [ -z $pathName ]
		then
			./2.sh 
		else
			./2.sh $pathName
		fi
                ;;

        "Create a hollowed square")
		chmod +x 3.sh
		read -p "Enter the sides: " var1 var2
		./3.sh $var1 $var2
                ;;
	"Find the words")
		chmod +x 4.sh
		read -p "Enter the wildcard and full pathaname(optional): " wildcard pathName
		if [ -z $pathName ]
		then
			./4.sh $wildcard
		else
			./4.sh $wildcard $pathName
		fi 
                ;;
        "Delete the empty files")
		chmod +x 5.sh
		read -p "Enter the keyword or pathname: " var
		./5.sh $var
                ;;
        "Exit")
		seq 1 15000 | while read i; do echo -en "\r$i"; done
		echo -en "\rSee you next time..."
		echo ""
            break
            ;;
        *) echo invalid option;;
    esac
done
