#ifndef SWITCHER_H
#define SWITCHER_H

#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

enum pieceType {Pawn, Knight, Bishop, Rook, Queen, King, Empty};
enum pieceColor {Black, Neutral, White};

class Switcher {
public:
	/*
		big-five, data members are not pointers so can use default!
	*/
	// destructor
	~Switcher() = default;

	// copy constructor
	Switcher(const Switcher & object) = default; 

	// move constructor
	Switcher( Switcher && object) = default; 

	// copy assignment
	Switcher & operator =(const Switcher & object) = default; 

	// move assignment
	Switcher & operator =(Switcher && object) = default; 

	// default constructor 
	Switcher();

	// constructor with piece initialization
	Switcher(unsigned int square, unsigned int value, pieceType type, pieceColor color); 

	// Mutator and accessor functions for determining/setting the piece type of an object
	pieceType getPieceType() const;
	void setPieceType(pieceType type);

	// Mutator and accessor functions for determining/setting the piece color of an object
	pieceColor getPieceColor() const;
	void setPieceColor(pieceColor color);

	// Mutator and accessor functions for determining/setting the piece value of an object
	unsigned int getPieceValue() const;
	void setPieceValue(unsigned int value);

	// Mutator and accessor functions for determining/setting the piece square of an object
	unsigned int getPieceSquare() const;
	void setPieceSquare(unsigned int square); // useful when moving a piece

	// Mutator and accessor functions for determining/setting the moving state of an object
	unsigned int getPieceMovevement() const;
	void setPieceMovement(bool movement);

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, vector<Switcher> & board) const;

	// if move is valid, make the move
	// On return, the piece's square value is updated
	bool makeMove(vector<Switcher> & board, int dest);

private:
	unsigned int square; // 0 - 63 for an 8x8 board with 0 being top left & 63 being bottom right
	unsigned int value; // pawn - 1, knight - 3, bishop - 3, rook - 5, queen - 9, king - infinity (use 10), empty - 0
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)

	// given a path (src -> dest) determine if there are peices in that path
	// returns "true" if path has only empty squares, else "false"
	bool pathEmptyRook(int src, int dest, vector<Switcher> & board) const;
	bool pathEmptyBishop(int src, int dest, vector<Switcher> & board) const;
	bool pathEmptyPawn(int src, int dest, vector<Switcher> & board) const;

};

// Board intialization
vector<Switcher> initBoard(unsigned int BOARD_SIZE);

// Print the current board position
void printBoard(const vector<Switcher> & v);

#endif // SWITCHER_H