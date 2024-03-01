# -----------------------------------------------
#                Parameters
# -----------------------------------------------
ifndef VERBOSE
.SILENT:
endif
.DEFAULT_GOAL: all
.PHONY: clean performance graphs
SHELL := /bin/bash


# -----------------------------------------------
#                 Functions ALL
# -----------------------------------------------
make experiment:
	chmod +x experiments.sh
	./experiments.sh

all: clean performance performancetestandset graphs

graphs:
	python3 Part1/python_scripts/plot.py
	python3 Part2/python_scripts/tasplot.py

clean_file:
	-@rm philosopher
	-@rm producer_consumer
	-@rm ReaderWriter
	-@rm test_and_set
	-@rm test_and_test_and_set
	-@rm 1

clean_csv:
	-rm Part1/csv/*.csv
	-rm Part2/csv/*.csv

clean_png:
	-rm Part1/png/*.png
	-rm Part2/png/*.png

clean: clean_file clean_csv clean_png

# -----------------------------------------------
#                 Functions PART.1
# -----------------------------------------------

philosopher:
	gcc ./Part1/src/philosopher.c -o philosopher -lpthread && ./philosopher 3

readerwriter:
	gcc ./Part1/src/ReaderWriter.c -o ReaderWriter -lpthread && ./ReaderWriter 3 3 

producerconsumer:
	gcc ./Part1/src/producer_consumer.c -o producer_consumer -lpthread && ./producer_consumer 3 3
	
performance:
	chmod +x performance.sh
	./performance.sh

# -----------------------------------------------
#                 Functions PART.2
# -----------------------------------------------

testandset:
	gcc ./Part2/src/test_and_set.c -o test_and_set -lpthread && ./test_and_set 3

testandtestandset:
	gcc ./Part2/src/test_and_test_and_set.c -o test_and_test_and_set -lpthread && ./test_and_test_and_set 3

performancetestandset:
	chmod +x performance_test_and_set.sh 
	./performance_test_and_set.sh
