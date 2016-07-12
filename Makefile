NAME=super_deduper
SDIR= source/
TDIR= test_suite/
CFLAGS= -pedantic -Wall -O3 -std=gnu++0x

.PHONY: clean default build distclean debug

default: build

build:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp \
	$(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -o $(NAME) $(CFLAGS)

test:
	cd $(TDIR); make; cd -

