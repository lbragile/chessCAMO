CXX = g++
CXXFLAGS  = -g -c -Wall
AFLAGS = -g -Wall

SFML_CFLAGS = -I ../../SFML/include/ -I ../include/ -I ../src/ -L ../../SFML/lib/
SFML_LFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -mwindows
CHESS_CFLAGS = -I ../include/

vpath %.cpp ../src
vpath %.h ../include

.PHONY: all
all: clean chess.o gui.o gui.exe

chess.o: chess.cpp chess.h
	$(CXX) $(CXXFLAGS) $(CHESS_CFLAGS) $<

gui.o: gui.cpp chess.h
	$(CXX) $(CXXFLAGS) $(SFML_CFLAGS) $<

gui.exe:
	$(CXX) $(AFLAGS) $(SFML_CFLAGS) chess.o gui.o -o chessCAMO $(SFML_LFLAGS)

clean:
	@echo "remove binaries from folder"
	rm *.o*