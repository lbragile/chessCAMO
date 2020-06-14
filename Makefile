# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall
GTEST_CFLAGS = -I ../googletest/googletest/include -L ../googletest/googletest/lib
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
GTEST_LFLAGS = -lgtest -lgtest_main
GCOV_LFLAGS = -lgcov

all_main: chess.o main.o main.exe
all_test: chess.o test.o test.exe
all_unit: chess.o unit.o unit.exe

chess.o: chess.cpp chess.h
	$(CC) $(CFLAGS) $(GCOV_CFLAGS) chess.cpp

test.o: test.cpp chess.h
	$(CC) $(CFLAGS) test.cpp

main.o: main.cpp chess.h
	$(CC) $(CFLAGS) main.cpp

unit.o: unit.cpp chess.h
	$(CC) $(CFLAGS) $(GTEST_CFLAGS) unit.cpp

test.exe:
	$(CC) $(AFLAGS) chess.o test.o -o test

main.exe:
	$(CC) $(AFLAGS) chess.o main.o -o main

unit.exe:
	$(CC) $(AFLAGS) $(GTEST_CFLAGS) chess.o unit.o -o unit $(GTEST_LFLAGS) $(GCOV_LFLAGS)

# generates the necessary files to make a code coverage report
# generates the report in both html and xml formats
# moves all generated gcov files to a folder called "gcov"
gcov:
	gcov chess.cpp
	gcovr -r . --exclude-throw-branches --html-details -o coverage.html
	gcovr -r . --exclude-throw-branches --xml -o coverage.xml

	mkdir "gcov"
	mv *.g* "gcov"

clean:
	@echo "clean project"
	rm *.g* ; rm -r "gcov" ; rm *.html ; rm *.o ; rm *.exe ;
	@echo "clean completed"