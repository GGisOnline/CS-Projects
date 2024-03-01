#!/bin/bash

programs=("philosopher" "ReaderWriter" "producer_consumer")
makecall=("philosopher" "readerwriter" "producerconsumer")
#we iterate through the 3 programms
for ((i=0; i<3; i++))
    do
    rm -f ./Part1/csv/mesures_${programs[$i]}.csv
    echo "thread,time" >> ./Part1/csv/mesures_${programs[$i]}.csv
    for thread in 1 2 4 8 16 32 64;
    do
        for (( r=0; r<5; r++))
	    		do  
	    			make ${makecall[$i]} -s 2> /dev/null
	    			if [ "ReaderWriter" == ${programs[$i]} ]
	    			then
	    				time=$(/usr/bin/time -f %e ./${programs[$i]} $thread $thread 2>&1 /dev/null)
	    			elif [ "philosopher" == ${programs[$i]} ]
	    			then
	    				time=$(/usr/bin/time -f %e ./${programs[$i]} $thread 2>&1 /dev/null)
	    			else
	    				time=$(/usr/bin/time -f %e ./${programs[$i]} $thread $thread 2>&1 /dev/null)
	    			fi
	    			
					make clean_file -s 2>1&>/dev/null

					#on l'écrit dans le fichier
	    			echo  $thread,$time >> ./Part1/csv/mesures_${programs[$i]}.csv

					
	    		done
    done
done