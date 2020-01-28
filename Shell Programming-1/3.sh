#!/bin/bash
#Creates nxn matrix
declare -A matrix
#checks if arguments are given or not
if [ -z "$2" ] || [ -z "$1" ]
then 
	echo "Your input is invalid.Try again.."
else
	rows=$1
	cols=$2
	even=`expr $1 - $2`
	var=$(($even % 2))
	if [ $1 -gt $2 ] && [ $var -eq 0 ];then
		#fills matrix with *
		for ((i=0;i<rows;i++)) do
		    for ((j=0;j<rows;j++)) do
			matrix[$i,$j]=*
		    done
		done
		#calculations for where to start deleting for inner matrix
		startRow1=`expr $rows - $cols`
		startRow=`expr $startRow1 / 2`
		startCol1=`expr $rows - $cols`
		startCol=`expr $startCol1 / 2`
		#fills mtrix wit " " (space)
		for ((k=0;k<cols;k++)) do
		    for ((l=0;l<cols;l++)) do
			matrix[$startRow,$startCol]=' '
			startRow=`expr $startRow + 1`
		    done
			startCol=`expr $startCol + 1`
			startRow1=`expr $rows - $cols`
			startRow=`expr $startRow1 / 2`
		done
        #prints matrix
		for ((m=0;m<rows;m++)) do
		    for ((n=0;n<rows;n++)) do
			echo -ne "${matrix[$m,$n]}"
		    done
		    echo 
		done
	else
		echo "Your input is invalid.Try again.."
			
	fi
fi
	
