# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -c -Wall
ALL_FLAGS = -g -Wall

FILE = chess
MAIN = main

all: $(FILE).o $(MAIN).o main

$(FILE).o: $(FILE).cpp $(FILE).h
	$(CC) $(CFLAGS) $(FILE).cpp

$(MAIN).o: $(MAIN).cpp $(FILE).cpp $(FILE).h
	$(CC) $(CFLAGS) $(MAIN).cpp

main:
	$(CC) $(ALL_FLAGS) $(FILE).o $(MAIN).o -o $(MAIN)

clean:
	@echo "clean project"
	-del *.o *.exe
	@echo "clean completed"