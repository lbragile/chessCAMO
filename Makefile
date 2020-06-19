CC = g++

CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall
GTEST_CFLAGS = -I ../googletest/googletest/include -L ../googletest/googletest/lib
CHESS_CFLAGS = -I include/
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
GTEST_LFLAGS = -lgtest -lgtest_main
GCOV_LFLAGS = -lgcov

# Selective Search for files in sub-directories
# https://www.gnu.org/software/make/manual/html_node/Selective-Search.html
vpath %.cpp src
vpath %.h include

all_main: chess.o main.o main.exe
all_unit: chess.o unit.o unit.exe

# $< (name of first pre-requisite) 
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html#Automatic-Variables
chess.o: chess.cpp chess.h
	$(CC) $(CFLAGS) $(CHESS_CFLAGS) $(GCOV_CFLAGS) $<

main.o: main.cpp chess.h
	$(CC) $(CFLAGS) $(CHESS_CFLAGS) $<

unit.o: unit.cpp chess.h
	$(CC) $(CFLAGS) $(GTEST_CFLAGS) $(CHESS_CFLAGS) $<

main.exe:
	$(CC) $(AFLAGS) chess.o main.o -o main $(GCOV_LFLAGS)

unit.exe:
	$(CC) $(AFLAGS) $(GTEST_CFLAGS) $(GCOV_CFLAGS) chess.o unit.o -o unit $(GTEST_LFLAGS) $(GCOV_LFLAGS)

.PHONY: gcov
gcov: chess.cpp
	gcov $<
	gcovr -r . --exclude-throw-branches --html-details -o coverage.html
	gcovr -r . --exclude-throw-branches --xml -o coverage.xml
	
	mkdir -p "gcov"
	mv *.html *.xml "gcov"

clean:
	@echo "clean project"
	(rm *.g* ; rm *.html ; rm *.o ; rm *.exe) || (-del *.g* ; -del *.html ; -del *.o ; -del *.exe)
	@echo "clean completed"