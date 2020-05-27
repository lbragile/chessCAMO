# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
ALL_FLAGS = -g -Wall

FILE = chess
TEST = test
MAIN = main

all_main: $(FILE).o $(MAIN).o main
all_test: $(FILE).o $(TEST).o test

$(FILE).o: $(FILE).cpp $(FILE).h
	$(CC) $(CFLAGS) $(FILE).cpp

$(TEST).o: $(TEST).cpp
	$(CC) $(CFLAGS) $(TEST).cpp

$(MAIN).o: $(MAIN).cpp
	$(CC) $(CFLAGS) $(MAIN).cpp

test:
	$(CC) $(ALL_FLAGS) $(FILE).o $(TEST).o -o $(TEST)

main:
	$(CC) $(ALL_FLAGS) $(FILE).o $(MAIN).o -o $(MAIN)

clean:
	@echo "clean project"
	-del *.o *.exe
	@echo "clean completed"