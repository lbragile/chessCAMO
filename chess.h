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
enum PlayerTurn{WhiteTurn, BlackTurn};
// enum pieceDirection {Row, Col, Diag_L, Diag_R};

class Piece; // forward declaration

class Chess
{
public:
	/*
		big-five, data members are not pointers so can use default!
	*/
	// destructor
	~Chess() = default;

	Chess(const Chess & object) = default; // copy constructor
	Chess(Chess && object) = default; // move constructor

	Chess & operator =(const Chess & object) = default; // copy assignment
	Chess & operator =(Chess && object) = default; // move assignment

	// default constructor 
	Chess();

	// Mutator and accessor functions for determining/setting the board's current state
	vector<Piece> getBoard() const {return board;}
	Chess setBoard(const vector<Piece> & board) {this->board = board; return *this;}

	// Mutator and accessor functions for determining/setting the board's checking piece/king
	stack<Piece> getCheckStack() const {return checkStack;}
	Chess setCheckStack(const stack<Piece> & checkStack) {this->checkStack = checkStack; return *this;}

	// Mutator and accessor functions for determining/setting the board's checking state 
	bool getCheck() const {return check;}
	Chess setCheck() {this->check = !check; return *this;}

	// Mutator and accessor functions for determining/setting the board's checkmate state
	bool getCheckmate() const {return checkmate;}
	Chess setCheckmate() {this->checkmate = !checkmate; return *this;}

	// Mutator and accessor functions for determining/setting the board's stalemate state
	bool getStalemate() const {return stalemate;}
	Chess setStalemate() {this->stalemate = !stalemate; return *this;}

	// Mutator and accessor functions for determining/setting the player's turn
	PlayerTurn getTurn() const {return turn;}
	Chess setTurn(PlayerTurn turn) {this->turn = turn; return *this;} // useful when moving a piece

	// if move is valid, make the move
	// On return, the piece's square value is updated
	void makeMove(int dest, vector<Piece> & board, int & turn, bool & valid);

	// Determines if the path from 'src' to 'dest' contains any pieces (Return 0) or not (Return 1)
	// Also makes sure that piece on 'dest' is not the same color as 'src'
	bool isPathFree(int src, int dest, const vector<Piece> & board);

	void isCheckmate(Piece king, Piece piece, vector<Piece> board);

	bool isStalemate(int turn, const vector<Piece> & board);

private:
	vector<Piece> board; // overall board state
	stack<Piece> checkStack; // needed to determine if a given player's king is in check
	bool checkmate;
	bool stalemate;
	bool check;
	PlayerTurn turn;

	// Decide if it is an attacking move or regular move
	vector<Piece> makeMoveForType(int src, int dest, vector<Piece> board);

	// for isCheckmate
	int pieceIterator(int src, int dest, Piece king, Piece piece, vector<Piece> board, int increment);

	// for isPathFree
	void pathIterator(int src, int dest, const vector<Piece> & board, int increment, bool & empty);
};

class Piece
{
public:
	/*
		big-five, data members are not pointers so can use default!
	*/
	// destructor
	virtual ~Piece() = default;

	Piece(const Piece & object) = default; // copy constructor
	Piece(Piece && object) = default; // move constructor

	Piece & operator =(const Piece & object) = default; // copy assignment
	Piece & operator =(Piece && object) = default; // move assignment

	// constructor with piece initialization
	Piece() : value{0}, type{Empty}, color{Neutral}, moved{false}, pinned{false} {};

	// constructor with piece initialization
	Piece(int value, pieceType type, pieceColor color);

	// Mutator and accessor functions for determining/setting the piece type of an object
	pieceType getPieceType() const {return type;}
	Piece setPieceType(pieceType type) {this->type = type; return *this;}

	// Mutator and accessor functions for determining/setting the piece color of an object
	pieceColor getPieceColor() const {return color;};
	Piece setPieceColor(pieceColor color) {this->color = color; return *this;}

	// Mutator and accessor functions for determining/setting the piece value of an object
	int getPieceValue() const {return value;}
	Piece setPieceValue(int value) {this->value = value; return *this;}

	// Mutator and accessor functions for determining/setting the piece square of an object
	int getPieceSquare() const {return square;}
	Piece setPieceSquare(int square) {this->square = square; return *this;} // useful when moving a piece

	// Mutator and accessor functions for determining/setting the moving state of an object
	bool getPieceMoveInfo() const {return moved;}
	Piece setPieceMoveInfo() {this->moved = !moved; return *this;}

	// Mutator and accessor functions for determining/setting if an object is pinned to king
	bool getPiecePinInfo() const {return pinned;}
	Piece setPiecePinInfo() {this->pinned = !pinned; return *this;}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	virtual bool isValidMove(int src, int dest, const vector<Chess> & board) {return isValidMove(src, dest, board);}

	virtual bool isFirstMove() {return isFirstMove();}

	virtual bool isPinned() {return isPinned();}


private:
	int square; // 0 - 63 for an 8x8 board with 0 being top left & 63 being bottom right
	int value; // pawn - 1, knight - 3, bishop - 3, rook - 5, queen - 9, king - infinity (use 10), empty - 0
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)
	bool pinned;
};

class Pawn : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	void promotePawn();
	bool isFirstMove();
};	

class Knight : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	bool isPinned();

};

class Bishop : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	bool isPinned();
};

class Rook : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	bool isFirstMove();
	bool isPinned();
};

class Queen : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	bool isPinned();
};

class King : public Piece
{
public:
	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Chess> & board);
	bool isFirstMove();
	bool canCastle();
	bool isChecked();
	bool isDoubleChecked();
	int numKingMoves();
};


/*****
	GLOBAL FUNCTIONS
 *****/

// Board intialization
void boardInit(Chess & chess);

// Print the current board position
void printBoard(const vector<Piece> & board);

// Change player's turn and print whose turn it is after a move is played
void playerTurn(int & turn);

// Updates the board as needed
void updatedBoardStatus(const vector<Piece> & board, Piece piece, int & turn, bool valid);

// resign or draw
void userEnded(int turn);

#endif // CHESS_H