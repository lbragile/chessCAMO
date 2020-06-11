# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall
GTEST_CFLAGS = -I ../googletest-master/googletest/include -I ../googletest-master/googlemock/include -L ../googletest-master/googletest/lib
GCOV_CFLAGS = -fprofile-arcs -ftest-coverage
GTEST_LFLAGS = -lgtest -lgmock -lgtest_main
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
	$(CC) $(CFLAGS) $(GTEST_CFLAGS) $(GCOV_CFLAGS) unit.cpp

test.exe:
	$(CC) $(AFLAGS) chess.o test.o -o test $(GCOV_LFLAGS)

main.exe:
	$(CC) $(AFLAGS) chess.o main.o -o main $(GCOV_LFLAGS)

unit.exe:
	$(CC) $(AFLAGS) $(GTEST_CFLAGS) chess.o unit.o -o unit $(GTEST_LFLAGS) $(GCOV_LFLAGS)

gcov:
	gcov chess.cpp >nul
	gcovr -r . --html -o convergence.html

	mkdir "gcov"
	move /y *.gcov "gcov" >nul
	move /y *.gcda "gcov" >nul
	move /y *.gcno "gcov" >nul

clean:
	@echo "clean project"
	rm *.o *.exe 
	rm *.gcno *.gcda *.html
	rmdir /S /Q "gcov"
	@echo "clean completed"