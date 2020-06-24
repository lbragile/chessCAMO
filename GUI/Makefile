CC = g++

CFLAGS  = -g -c -Wall
AFLAGS = -g -Wall

SFML_CFLAGS = -I ../../SFML/include/ -I ../include/ -I ../src/ -L ../../SFML/lib/
SFML_LFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -mwindows

vpath %.cpp ../src
vpath %.h ../include

all: gui

.PHONY: gui
gui: gui.cpp chess.cpp
	$(CC) $(AFLAGS) $(SFML_CFLAGS) $^ -o gui $(SFML_LFLAGS)
