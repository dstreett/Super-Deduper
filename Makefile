PROGRAM_NAME = super_deduper
VERSION = 1.40
CC = g++
CFLAGS = -g -lz -DVERSION=$(VERSION)
DEBUG = -g
OPT = -O3
ARCHIVE = $(PROGRAM_NAME)_$(VERSION)
LDFLAGS=
LIBS = -lz
SDIR = source

.PHONY: clean default build distclean dist debug

default: build

Binary_Search_Tree.o: $(SDIR)/Binary_Search_Tree.cpp $(SDIR)/Binary_Search_Tree.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.cpp

Binary_Search_Tree_Memory_Eff_Node.o: $(SDIR)/Binary_Search_Tree_Memory_Eff_Node.cpp $(SDIR)/Binary_Search_Tree_Memory_Eff_Node.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.cpp

Binary_Search_Tree_Reads_Node.o: $(SDIR)/Binary_Search_Tree_Reads_Node.cpp $(SDIR)/Binary_Search_Tree_Reads_Node.h
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.cpp

main.o: $(SDIR)/main.cpp $(SDIR)/Binary_Search_Tree.cpp $(SDIR)/Binary_Search_Tree_Reads_Node.cpp $(SDIR)/Binary_Search_Tree_Memory_Eff_Node.cpp
	$(CC) $(CFLAGS) $(OPT) -c $(SDIR)/$*.cpp

clean:
	rm -rf *.o $(SDIR)/*.gch super_deduper

distclean: clean
	rm -rf *.tar.gz

dist:
	tar -zcf $(ARCHIVE).tar.gz source scripts examples super_deduper README Makefile

build: Binary_Search_Tree.o Binary_Search_Tree_Memory_Eff_Node.o Binary_Search_Tree_Reads_Node.o main.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(OPT) $? -o super_deduper $(LIBS)

debug:
	$(MAKE) build "CFLAGS=-Wall -pedantic -g -DDEBUG"

