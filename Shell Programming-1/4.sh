#!/bin/bash

if [ $# == 1 ]; then
    wildcard=${1//\"/}
    echo $wildcard
    dir=$(pwd)
    for i in $dir/*.txt
    do
        # Get the file name
        fileName="${i##*/}"
        while read -ra line; 
        do  
            #seperate the lines word by word
            for original in "${line[@]}";
            do  
                #check if the wildcard derive inside string
                if [[ "$original" = $wildcard ]]
                then
                    #make string uppercase
                    new=${original^^}
                    #change existing string by new one
                    sed -i "s/$original/$new/g" $fileName
                    echo "The word “$original” inside $fileName is substituted with “$new”."

                fi
            done;
        done < "$fileName"
    done
    
elif [ $# -gt 1 ]; then
    
    echo $2
    dir=$2
    wildcard=${1//\"/}
    echo $wildcard
    
    for i in $dir/*.txt
    do
        # Get the file name
        fileName="${i##*/}"
        while read -ra line; 
        do  
            #seperate the lines word by word
            for original in "${line[@]}";
            do  
                #check if the wildcard derive inside string
                if [[ "$original" = $wildcard ]]
                then
                    #make string uppercase
                    new=${original^^}
                    #change existing string by new one
                    sed -i "s/$original/$new/g" $fileName
                    echo "The word “$original” inside $fileName is substituted with “$new”."

                fi
            done;
        done < "$fileName"
    done
    
    
else
    echo "Please Give arguments"
fi

