NAME=super_deduper
CC=g++
SDIR= source/
TDIR= test_suite/

.PHONY: clean default build distclean debug

default: build


build:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp $(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -O3 -o $(NAME)

test:
	cd $(TDIR); make; cd -

