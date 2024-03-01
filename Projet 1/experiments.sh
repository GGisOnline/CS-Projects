#!/bin/bash

programs=("producer_consumer")
makecall=("producerconsumer")
#we iterate through the 3 programms
for ((i=0; i<1; i++))
    do
	echo ${programs[$i]}
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

					#on l'écrit dans le fichier
					echo  $thread,$time 
					
	    		done
    done
done
