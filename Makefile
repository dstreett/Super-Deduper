NAME=super_deduper
SDIR= source/
TDIR= test_suite/
CFLAGS= -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy \
		-Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations \
		-Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual \
		-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel \
		-Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused \
		-Wmaybe-uninitialized -Wuninitialized -O3 -std=gnu++0x

.PHONY: clean default build distclean debug

default: build

build:
	g++ $(SDIR)main.cpp $(SDIR)fileHelper.cpp $(SDIR)readInfo.cpp $(SDIR)argCollector.cpp \
	$(SDIR)fileWriter.cpp $(SDIR)binarySearch.cpp -o $(NAME) -Wmaybe-uninitialized -Wuninitialized -O3 -std=gnu++0x

test:
	cd $(TDIR); make; cd -

