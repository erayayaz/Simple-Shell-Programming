#!/bin/bash
#checks if an argument is given or not
if [ $# -gt 0 ];
then	
    #checks if keyword given right
	if [ "$1" == "-R" ] || [ "$1" == "-r" ]; then #if input is a keyword
		var=$(find ~ -empty -type f | wc -l)
		#if there are files under directory
		if [ $var -gt 0 ];then
			walk_dir () {
                #searchs all files under directory
			    for pathname in "$1"/*; do
			    #checks a pathname is directory or folder
				if [ -d "$pathname" ]; then
				    walk_dir "$pathname"
				else
                    #checks if given file is empty or not
					if [ -s "$pathname" ]
					then
				    		continue
					else
						read -p "Do you want to delete ${pathname##*/}? (y/n): " choice
						if [ "$choice" == "y" ] || [ "$choice" == "Y" ]
						then
                            #deletes file if user want so
							echo "1 file deleted"
							rm "$pathname"
							echo " "
						elif [ "$choice" == "n" ] || [ "$choice" == "N" ]
						then
							echo " "
						else
							echo "Please just give a y/n for input."
						fi		
					fi
				fi
			    done
			}
			Dir=$(pwd)
			walk_dir "$Dir"
		else
			echo "There is no empty files inside the directory"
		fi
	elif [ -d "$1" ]; then #İf input is a pathname
		dir=$1
		var=$(find ~ -maxdepth 1 -empty -type f | wc -l)
		#if there are files under directory
		if [ $var -gt 0 ];then
            #checks if there is empty file or not
			for i in $(find $dir -maxdepth 1 -type f -empty)
			do	
				read -p "Do you want to delete ${i##*/}? (y/n): " choice
				if [ "$choice" == "y" ] || [ "$choice" == "Y" ]
				then	
                    #deletes file if user want so
					echo "1 file deleted"
					rm "$i"
					echo " "
				elif [ "$choice" == "n" ] || [ "$choice" == "N" ]
				then
					echo " "
				else
					echo "Please just give a y/n for input."
					echo " "
				fi	
			done
		else
			echo "There is no empty files inside the directory"
		fi
	else
		echo "Your input is invalid.Please try again."
	fi
else
	echo "Your input is invalid.Please try again."
fi
