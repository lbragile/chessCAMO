CC = g++

CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall

SFML_CFLAGS = -I ../../SFML/include/ -I ../include/ -I ../src/ -L ../../SFML/lib/
SFML_LFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

vpath %.cpp ../src
vpath %.h ../include

all: gui.o gui.exe

gui.o: gui.cpp
	$(CC) $(CFLAGS) $(SFML_CFLAGS) $<

gui.exe:
	$(CC) $(AFLAGS) $(SFML_CFLAGS) gui.o -o gui $(SFML_LFLAGS)