#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <stdlib.h>     /* abs */

using namespace std;

enum pieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY}; // caps to avoid class name conflict
enum pieceColor {Black, Neutral, White};
enum PlayerTurn{WhiteTurn, BlackTurn};
// enum pieceDirection {Row, Col, Diag_L, Diag_R};

// forward declaration
class Piece; 
class Pawn;
class Knight;
class Bishop;
class Rook;
class Queen;
class King; 

class Chess
{
public:
	/*
		big-five, data members are not pointers so can use default!
	*/
	
	~Chess() = default; // destructor
	Chess(const Chess & object) = default; // copy constructor
	Chess(Chess && object) = default; // move constructor
	Chess & operator =(const Chess & object) = default; // copy assignment
	Chess & operator =(Chess && object) = default; // move assignment

	// default constructor 
	Chess() : checkmate{false}, stalemate{false}, check{false}, turn{WhiteTurn} {}

	// Mutator and accessor functions for determining/setting the board's current state
	vector<Piece*> getBoard() const {return board;}
	void setBoard(const vector<Piece*> & board) {this->board = board;}

	// Mutator and accessor functions for determining/setting the board's checking piece/king
	stack<Piece*> getCheckStack() const {return checkStack;}
	void setCheckStack(const stack<Piece*> & checkStack) {this->checkStack = checkStack;}

	// Mutator and accessor functions for determining/setting the board's checking state 
	bool getCheck() const {return check;}
	void setCheck() {this->check = !check;}

	// Mutator and accessor functions for determining/setting the board's checkmate state
	bool getCheckmate() const {return checkmate;}
	void setCheckmate() {this->checkmate = !checkmate;}

	// Mutator and accessor functions for determining/setting the board's stalemate state
	bool getStalemate() const {return stalemate;}
	void setStalemate() {this->stalemate = !stalemate;}

	// Mutator and accessor functions for determining/setting the player's turn
	PlayerTurn getTurn() const {return turn;}
	void setTurn(PlayerTurn turn) {this->turn = turn;} // useful when moving a piece

	// if move is valid, make the move
	// On return, the piece's square value is updated
	void makeMove(int dest, vector<Piece> & board, int & turn, bool & valid);

	// Determines if the path from 'src' to 'dest' contains any pieces (Return 0) or not (Return 1)
	// Also makes sure that piece on 'dest' is not the same color as 'src'
	bool isPathFree(int src, int dest, const vector<Piece> & board);

	void isCheckmate(Piece king, Piece piece, vector<Piece> board);

	bool isStalemate(int turn, const vector<Piece> & board);

private:
	vector<Piece*> board; // overall board state
	stack<Piece*> checkStack; // needed to determine if a given player's king is in check
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
	virtual ~Piece() = default; // destructor
	Piece(const Piece & object) = default; // copy constructor
	Piece(Piece && object) = default; // move constructor
	Piece & operator =(const Piece & object) = default; // copy assignment
	Piece & operator =(Piece && object) = default; // move assignment

	// constructor with piece initialization
	Piece() : value{0}, type{EMPTY}, color{Neutral}, moved{false}, pinned{false} {}

	// constructor with piece initialization
	Piece(int value, pieceType type, pieceColor color) : moved{false}, pinned{false}
	{this->value = value; this->type = type; this->color = color;}

	// Mutator and accessor functions for determining/setting the piece type of an object
	pieceType getPieceType() const {return type;}
	void setPieceType(pieceType type) {this->type = type;}

	// Mutator and accessor functions for determining/setting the piece color of an object
	pieceColor getPieceColor() const {return color;}
	void setPieceColor(pieceColor color) {this->color = color;}

	// Mutator and accessor functions for determining/setting the piece value of an object
	int getPieceValue() const {return value;}
	void setPieceValue(int value) {this->value = value;}

	// Mutator and accessor functions for determining/setting the moving state of an object
	bool getPieceMoveInfo() const {return moved;}
	void setPieceMoveInfo() {this->moved = !moved;}

	// Mutator and accessor functions for determining/setting if an object is pinned to king
	bool getPiecePinInfo() const {return pinned;}
	void setPiecePinInfo() {this->pinned = !pinned;}

	bool isEmpty() {return this->getPieceType() == EMPTY;}
	bool isPawn() {return this->getPieceType() == PAWN;}
	bool isKnight() {return this->getPieceType() == KNIGHT;}
	bool isBishop() {return this->getPieceType() == BISHOP;}
	bool isRook() {return this->getPieceType() == ROOK;}
	bool isQueen() {return this->getPieceType() == QUEEN;}
	bool isKing() {return this->getPieceType() == KING;}

	bool isPieceWhite() {return this->getPieceColor() == White;}
	bool isPieceBlack() {return this->getPieceColor() == Black;}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int dest, vector<Piece> board);

private:
	int value; // pawn - 1, knight - 3, bishop - 3, rook - 5, queen - 9, king - infinity (use 10), empty - 0
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)
	bool pinned;
};

class Pawn : public Piece
{
public:
	// constructor with piece initialization
	Pawn() : Piece() {}

	// constructor with piece initialization
	Pawn(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isLegalMove(int dest, vector<Piece> board);
	void promotePawn();
	bool isFirstMove();
};	

class Knight : public Piece
{
public:
	// constructor with piece initialization
	Knight() : Piece() {}

	// constructor with piece initialization
	Knight(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Piece> & board);
	bool isPinned();

};

class Bishop : public Piece
{
public:
	// constructor with piece initialization
	Bishop() : Piece() {}

	// constructor with piece initialization
	Bishop(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Piece> & board);
	bool isPinned();
};

class Rook : public Piece
{
public:
	// constructor with piece initialization
	Rook() : Piece() {}

	// constructor with piece initialization
	Rook(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Piece> & board);
	bool isFirstMove();
	bool isPinned();
};

class Queen : public Piece
{
public:
	// constructor with piece initialization
	Queen() : Piece() {}

	// constructor with piece initialization
	Queen(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Piece> & board);
	bool isPinned();
};

class King : public Piece
{
public:
	// constructor with piece initialization
	King() : Piece() {}

	// constructor with piece initialization
	King(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}

	// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
	// Return 'true' if move is valid, 'false' otherwise
	bool isValidMove(int src, int dest, const vector<Piece> & board);
	bool isFirstMove();
	bool canCastle();
	bool isChecked();
	bool isDoubleChecked();
	int numKingMoves();
};

class Empty : public Piece
{
public:
	// constructor with piece initialization
	Empty() : Piece() {}

	// constructor with piece initialization
	Empty(int value, pieceType type, pieceColor color) : Piece(value, type, color) {}
};


/*****
	GLOBAL FUNCTIONS
 *****/

// Board intialization
void boardInit(Chess & chess);

// Print the current board position
void printBoard(const vector<Piece*> & board);

// Change player's turn and print whose turn it is after a move is played
void playerTurn(int & turn);

// Updates the board as needed
void updatedBoardStatus(const vector<Piece> & board, Piece piece, int & turn, bool valid);

// resign or draw
void userEnded(int turn);

#endif // CHESS_H