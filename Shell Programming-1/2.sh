#!/bin/bash
var=$1
dir=$(pwd)
	if [ ! -n "$var" ] #İt is checking the inputs if it there is any inputs with .sh file it will do their jobs.
	then
	b="cprogs"
        if [ -d $b ] #This if checking the directory if there any cprogs directory created before that.
        then	
            for i in $dir/* #This loops check the subdirectory and if they have any .c files. They will move into to the cprogs.
                do
                #get the file name
                fileName="${i##*/}"
                if [ -d "$dir/$fileName" ]; then #This check the this files is a directory or not.
                    for i in $dir/$fileName/*.c #This loop walk around the directory and find the .c files.
                    do
                        subFileName="${i##*/}"
                        confirmer=0
                        if [ "$fileName" == "cprogs" ] || [ "$subFileName" == "*.c" ];then
                            continue
                        else	
                            if [ -d "$subFileName" ]; #If it is a directory keep moving around the directory if it is not move the .c files to cprogs.
                            then
                                continue
                            else
                                mv $dir/$fileName/$subFileName $b
                                confirmer=1
                            fi
                        fi	
                    done				
                fi
                
            done
            controller=0
            for i in $dir/*.c  #This loops check the files and if there are any .c files. They will move into to the cprogs.
                do
                    fileName="${i##*/}"
                    if [ -d "$b" ]
                    then	
                        if [ "$fileName" == "*.c" ] #It check the error controls because sometimes empty files seems like a *.c .
                        then
                            continue
                        else
                            mv $dir/$fileName $b
                            controller=1
                        fi	
                    else
                        continue
                    fi	
                done
        else
            `mkdir $b`
            echo "A directory named cprogs is created under current working directory."
            for i in $dir/* #This loops check the subdirectory and if they have any .c files. They will move into to the cprogs.
                do
                #get the file name
                fileName="${i##*/}"
                if [ -d "$dir/$fileName" ]; then
                    for i in $dir/$fileName/*.c
                    do
                        subFileName="${i##*/}"
                        confirmer=0
                        if [ "$fileName" == "cprogs" ] || [ "$subFileName" == "*.c" ];then
                            continue
                        else	
                            if [ -d "$subFileName" ];
                            then
                                continue
                            else
                                mv $dir/$fileName/$subFileName $b
                                confirmer=1
                            fi
                        fi	
                    done				
                fi
                
            done
            controller=0
            for i in $dir/*.c  #This loops check the files and if there are any .c files. They will move into to the cprogs.
                do
                    fileName="${i##*/}"
                    if [ -d "$b" ]
                    then	
                        if [ "$fileName" == "*.c" ]
                        then
                            continue
                        else
                            mv $dir/$fileName $b
                            controller=1
                        fi	
                    else
                        continue
                    fi	
                done
        fi
	else
        b="/cprogs"
	c="${var}${b}"
        if [ -d $c ] #Check the cprogs directory created before the start code.
        then	
            for i in $var/* #This loops check the subdirectory and if they have any .c files. They will move into to the cprogs.
	    	do
			#get the file name
			fileName="${i##*/}"
			if [ -d "$var/$fileName" ]; then
				for i in $var/$fileName/*.c
				do
					subFileName="${i##*/}"
					confirmer=0
					path1=0
					if [ "$fileName" == "cprogs" ] || [ "$subFileName" == "*.c" ];then
						continue
					else	
						
						if [ -d "$subFileName" ];
						then
							continue
						else
							mv $var/$fileName/$subFileName $c
							confirmer=1
							path1=1
						fi
					fi	
				done				
			fi
			
		done
		controller=0
		path=0
		for i in $var/*.c  #This loops check the files and if there are any .c files. They will move into to the cprogs.
			do
				fileName="${i##*/}"
				if [ -d "$c" ]
				then	
					if [ "$fileName" == "*.c" ] #It check the error control.
					then
						continue
					else
						mv $var/$fileName $c
						controller=1
						path=1
					fi	
				else
					continue
				fi	
			done
        else
		b="/cprogs"
		c="${var}${b}"
		`mkdir $c`
		echo "A directory named cprogs is created under current working directory."

		for i in $var/* #This loops check the subdirectory and if they have any .c files. They will move into to the cprogs.
	    	do
			#get the file name
			fileName="${i##*/}"
			if [ -d "$var/$fileName" ]; then
				for i in $var/$fileName/*.c
				do
					subFileName="${i##*/}"
					confirmer=0
					path1=0
					if [ "$fileName" == "cprogs" ] || [ "$subFileName" == "*.c" ];then
						continue
					else	
						
						if [ -d "$subFileName" ]; #If it is a file it will keep movin around the subdirectory if it is not it will move the .c files to cprogs.
						then
							continue
						else
							mv $var/$fileName/$subFileName $c
							confirmer=1
							path1=1
						fi
					fi	
				done				
			fi
			
		done
		controller=0
		path=0
		for i in $var/*.c  #This loops check the files and if there are any .c files. They will move into to the cprogs.İt will be in the current directory.
			do
				fileName="${i##*/}"
				if [ -d "$c" ]
				then	
					if [ "$fileName" == "*.c" ]
					then
						continue
					else
						mv $var/$fileName $c
						controller=1
						path=1
					fi	
				else
					continue
				fi	
			done
        fi
	fi
	if [ "$confirmer" == "1" ] || [ "$controller" == "1" ]; #This variables check the .c files if they are moved it will print the message.
	then	
		if [ "$path" == "1" ] || [ "$path1" == "1" ]; #This variables are checking the input if input is a pathname it will print the message with pathname.
		then
			echo "All the files whose name ends with .c in $var are moved into cprogs directory."
		else
			echo "All the files whose name ends with .c in the current working directory are moved into cprogs directory."
		fi
	fi

