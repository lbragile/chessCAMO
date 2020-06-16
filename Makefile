# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall
GTEST_CFLAGS = -I ../googletest/googletest/include -L ../googletest/googletest/lib
CHESS_CFLAGS = -I ./include/
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
GTEST_LFLAGS = -lgtest -lgtest_main
GCOV_LFLAGS = -lgcov

all_main: chess.o main.o main.exe
all_unit: chess.o unit.o unit.exe
# all_test: chess.o test.o test.exe

chess.o: src/chess.cpp include/chess.h
	$(CC) $(CFLAGS) $(GCOV_CFLAGS) $(CHESS_CFLAGS) src/chess.cpp

main.o: src/main.cpp include/chess.h
	$(CC) $(CFLAGS) $(CHESS_CFLAGS) src/main.cpp

unit.o: src/unit.cpp include/chess.h
	$(CC) $(CFLAGS) $(GTEST_CFLAGS) $(CHESS_CFLAGS) src/unit.cpp

# test.o: test.cpp chess.h
# 	$(CC) $(CFLAGS) test.cpp
# 	

main.exe:
	$(CC) $(AFLAGS) chess.o main.o -o main $(GCOV_LFLAGS)

unit.exe:
	$(CC) $(AFLAGS) $(GTEST_CFLAGS) chess.o unit.o -o unit $(GTEST_LFLAGS) $(GCOV_LFLAGS)

# test.exe:
# 	$(CC) $(AFLAGS) chess.o test.o -o test $(GCOV_LFLAGS)

# generates the necessary files to make a code coverage report
# generates the report in both html and xml formats
# moves all generated gcov files to a folder called "gcov"
gcov:
	gcov chess.cpp
	gcovr -r . --exclude-throw-branches --html-details -o coverage.html
	gcovr -r . --exclude-throw-branches --xml -o coverage.xml
	
	mkdir "gcov"
	mv *.g* *.html *.xml "gcov"

clean:
	@echo "clean project"
	rm *.g* ; rm -r "gcov" ; rm *.html ; rm *.o ; rm *.exe ;
	@echo "clean completed"