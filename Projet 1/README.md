# SysInfo_P1
First Project for LINFO1252 
Authors: Tim Krämer et Jérome Lechat


#In our zip.file we have  the following files:

#First Part 


make philosopher 
#executes the philosopher programm with 3 threads

make readerwriter 
#executes the readerwriter programm with 3 threads

make producerconsumer:
#executes the producer_consumer programm with 3 threads

make performance:
#be carefull when executing, readerwriter tests take quite some time !
#executes the performance script which executes the 3 programms before 5 times each, with a diffrent amound of threads and writes those into a csv sheat.


#Second Part 


make testandset
#executes the testandset programm with 3 threads

make testandtestandset
#executes the testandtestandset programm with 3 threads

make performancetestandset
#executes the performance script which executes the tas and tatas programm 5 times each, with a diffrent amound of threads and writes those into a csv sheat.


#Additional fun


make experiment
#executes the script experiments.sh which we needed for the inginious submission

make graphs
#executes python scripts which plot the data from the csv files created from make performance and make performancetestandset.

make clean_file
#cleans programm files

make clean_csv
#cleans csv files

make clean_png
#cleans png files

make clean
#make clean_file make clean_csv make clean_png

make all
#make clean make performance make performancetestandset make graphs
#take a coffe while executing 

