CXX = g++

# CXXFLAGS  = -g -c -Wall
CXXFLAGS = -g -Wall

SFML_CFLAGS = -I ../../SFML/include/ -I ../include/ -I ../src/ -L ../../SFML/lib/
SFML_LFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -mwindows

vpath %.cpp ../src
vpath %.h ../include

all: gui

.PHONY: gui
gui: gui.cpp chess.cpp
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) $^ -o chessCAMO $(SFML_LFLAGS)
