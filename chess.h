#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <stdlib.h>     /* abs */

using namespace std;

enum pieceType {Pawn, Knight, Bishop, Rook, Queen, King, Empty};
enum pieceColor {Black, Neutral, White};
enum pieceDirection {Row, Col, Diag_L, Diag_R};

class Chess {
public:
	/*
		big-five, data members are not pointers so can use default!
	*/
	// destructor
	~Chess() = default;

	// copy constructor
	Chess(const Chess & object) = default; 

	// move constructor
	Chess( Chess && object) = default; 

	// copy assignment
	Chess & operator =(const Chess & object) = default; 

	// move assignment
	Chess & operator =(Chess && object) = default; 

	// default constructor 
	Chess();

	// constructor with piece initialization
	Chess(unsigned int square, unsigned int value, pieceType type, pieceColor color); 

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
	bool getPieceMovevement() const;
	void setPieceMovement();

	// if move is valid, make the move
	// On return, the piece's square value is updated
	void makeMove(int dest, vector<Chess> & board, int & turn, bool & valid);

private:
	unsigned int square; // 0 - 63 for an 8x8 board with 0 being top left & 63 being bottom right
	unsigned int value; // pawn - 1, knight - 3, bishop - 3, rook - 5, queen - 9, king - infinity (use 10), empty - 0
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);

	// Determines if the path from 'src' to 'dest' contains any pieces (Return 0) or not (Return 1)
	// Also makes sure that piece on 'dest' is not the same color as 'src'
	bool isPathFree(int src, int dest, const vector<Chess> & board);

	bool isCastled(int src, int dest, const vector<Chess> & board);

	// A valid move was made
	// Return true if king is in check, otherwise return false
	bool isCheck(int src, int dest, vector<Chess> board);

	void isCheckmate(Chess king, Chess piece, vector<Chess> board);

	// Decide if it is an attacking move or regular move
	vector<Chess> makeMoveForType(int src, int dest, vector<Chess> board);

	// When a pawn reaches the end of the board, it can be exchanged for either a queen, rook, bishop or knight
	void promotePawn();

	// for isCheckmate
	int pieceIterator(int src, int dest, Chess king, Chess piece, vector<Chess> board, int increment);

	// for isPathFree
	void pathIterator(int src, int dest, const vector<Chess> & board, int increment, bool & empty);

	int numberOfKingMoves(int src, int dest, vector<Chess> board, int counter);
};

extern stack<Chess> checkStack; // needed to determine if a given player's king is in check
extern bool checkmate;
extern bool stalemate;
extern bool check;

// Board intialization
void initBoard(vector<Chess> & board);

// Print the current board position
void printBoard(const vector<Chess> & board);

// Change player's turn and print whose turn it is after a move is played
void playerTurn(int & turn);

// Updates the board as needed
void updatedBoardStatus(const vector<Chess> & board, Chess piece, int & turn, bool valid);

void userEnded(int turn);

#endif // CHESS_H