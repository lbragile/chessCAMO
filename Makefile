# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
ALL_FLAGS = -g -Wall
GTEST_CFLAGS = -I ../googletest-master/googletest/include -L ../googletest-master/googletest/lib -fprofile-arcs -ftest-coverage
LFLAGS = -lgtest -lgcov

all_main: chess.o main.o main.exe
all_test: chess.o test.o test.exe
all_unit: unit.o unit.exe gcov

chess.o: chess.cpp chess.h
	$(CC) $(CFLAGS) chess.cpp

test.o: test.cpp chess.h
	$(CC) $(CFLAGS) test.cpp

main.o: main.cpp chess.h
	$(CC) $(CFLAGS) main.cpp

unit.o: unit.cpp chess.cpp
	$(CC) $(CFLAGS) $(GTEST_CFLAGS) unit.cpp

test.exe:
	$(CC) $(ALL_FLAGS) chess.o test.o -o test
	test

main.exe:
	$(CC) $(ALL_FLAGS) chess.o main.o -o main
	main

unit.exe:
	$(CC) $(ALL_FLAGS) $(GTEST_CFLAGS) unit.o $(LFLAGS) -o unit
	unit --gtest_color=yes

gcov:
	gcov unit.cpp >nul
	gcovr -r . --html -o convergence.html

	mkdir "gcov"
	move /y *.gcov "gcov" >nul
	move /y *.gcda "gcov" >nul
	move /y *.gcno "gcov" >nul

clean:
	@echo "clean project"
	-del *.o *.exe *.html
	rmdir /S /Q "gcov"
	@echo "clean completed"