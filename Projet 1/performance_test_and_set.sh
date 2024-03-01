#!/bin/bash

programs=("test_and_set" "test_and_test_and_set")
makecall=("testandset" "testandtestandset")
#we iterate through the 2 programms
for ((i=0; i<2; i++))
    do
    rm -f ./Part2/csv/mesures_${programs[$i]}.csv
    echo "thread,time" >> ./Part2/csv/mesures_${programs[$i]}.csv
    for thread in 1 2 4 6 8;
    do
        for (( r=0; r<5; r++))
	    		do  
	    			make ${makecall[$i]} -s 2>1&>/dev/null
	    			if [ "test_and_set" == ${programs[$i]} ]
	    			then
	    				time=$(/usr/bin/time -f %e ./${programs[$i]} $thread 2>&1 /dev/null)
	    			else
	    				time=$(/usr/bin/time -f %e ./${programs[$i]} $thread 2>&1 /dev/null)
	    			fi
	    			#on l'écrit dans le fichier

					make clean_file -s 2>1&>/dev/null

	    			echo  $thread,$time >> ./Part2/csv/mesures_${programs[$i]}.csv

					
	    		done
    done
done