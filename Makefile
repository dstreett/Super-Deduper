NAME=super_deduper
CC=g++
SDIR= source/
TDIR= test_suite/

.PHONY: clean default build distclean debug

default: all


all:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -O3 -o $(NAME)

test:
	cd $(TDIR); make; cd -

clean:
	rm super_deduper

debug:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -ggdb -o $(NAME)

valgrind:
	valgrind ./super_deduper -F -1 ./test_suite/fastqFiles/testCase_1X_R1.fastq -2 ./test_suite/fastqFiles/testCase_1X_R1.fastq -O
