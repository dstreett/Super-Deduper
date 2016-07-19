NAME=super_deduper
CC=g++
SDIR= source/
TDIR= test_suite/

.PHONY: clean default build distclean debug

default: all


all:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -Wall -O3 -std=c++11 -o $(NAME)

test:
	cd $(TDIR); make; cd -

clean:
	rm super_deduper

debug:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -Wall -g -std=c++11 -o $(NAME)

valgrind:
	valgrind ./super_deduper -F -1 ./test_suite/fastqFiles/testCase_1X_R1.fastq -2 ./test_suite/fastqFiles/testCase_1X_R1.fastq -O

clang:
	clang++-3.7 $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -Weverything -Wno-c++98-compat-pedantic -Wno-padded -g -std=c++11 -o $(NAME)
