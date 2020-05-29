#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <stdlib.h>     /* abs */
#include <fstream>

using namespace std;

enum pieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY}; // caps to avoid class name conflict
enum pieceColor {BLACK, NEUTRAL, WHITE};
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
	~Chess(); // destructor
	Chess(const Chess & object) = default; // copy constructor
	Chess & operator =(const Chess & object) = default; // copy assignment

	// default constructor 
	Chess() : checkmate{false}, stalemate{false}, check{false}, turn{WHITE} {}

	// Mutator and accessor functions for determining/setting the board's current state
	vector<Piece*> getBoard() const {return board;}
	void setBoard(const vector<Piece*> & board) {this->board = board;}

	// Mutator and accessor functions for determining/setting the board's checking piece/king
	stack<Piece*> getCheckStack() const {return checkStack;}
	void setCheckStack(const stack<Piece*> & checkStack) {this->checkStack = checkStack;}

	// Mutator and accessor functions for determining/setting the board's checking state 
	bool getCheck() const {return check;}
	void setCheck(bool check) {this->check = check;}

	// Mutator and accessor functions for determining/setting the board's checkmate state
	bool getCheckmate() const {return checkmate;}
	void setCheckmate(bool checkmate) {this->checkmate = checkmate;}

	// Mutator and accessor functions for determining/setting the board's stalemate state
	bool getStalemate() const {return stalemate;}
	void setStalemate(bool stalemate) {this->stalemate = stalemate;}

	// Mutator and accessor functions for determining/setting the player's turn
	pieceColor getTurn() const {return turn;}
	void setTurn(pieceColor turn) {this->turn = turn;} // useful when moving a piece

	void makeMove(int src, int dest);
	void isCheckmate();
	bool isStalemate(int turn, const vector<Piece> & board);

	bool isPathFree(int src, int dest);
	
private:
	vector<Piece*> board; // overall board state
	stack<Piece*> checkStack; // needed to determine if a given player's king is in check
	bool checkmate;
	bool stalemate;
	bool check;
	pieceColor turn;

protected:
	// Decide if it is an attacking move or regular move
	void makeMoveForType(int src, int dest);

	// for isCheckmate
	bool pieceIterator(int src, int dest);

	// for isPathFree
	bool pathIterator(int src, int dest, int increment);

	int incrementChoice(int & src, int & dest);
	void handleCheckmate();
};

class Piece : public Chess
{
public:
	virtual ~Piece(); // destructor
	Piece(const Piece & object) = default; // copy constructor
	Piece & operator =(const Piece & object) = default; // copy assignment

	// constructor with piece initialization
	Piece() : square{0}, value{0}, type{EMPTY}, color{NEUTRAL}, moved{false} {}

	// constructor with piece initialization
	Piece(int square, int value, pieceType type, pieceColor color) : moved{false}
	{this->square = square; this->value = value; this->type = type; this->color = color;}

	// Mutator and accessor functions for determining/setting the piece value of an object
	int getPieceSquare() const {return square;}
	void setPieceSquare(int square) {this->square = square;}

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
	void setPieceMoveInfo(bool moved) {this->moved = moved;}

	bool isEmpty() {return this->getPieceType() == EMPTY;}
	bool isPawn() {return this->getPieceType() == PAWN;}
	bool isKnight() {return this->getPieceType() == KNIGHT;}
	bool isBishop() {return this->getPieceType() == BISHOP;}
	bool isRook() {return this->getPieceType() == ROOK;}
	bool isQueen() {return this->getPieceType() == QUEEN;}
	bool isKing() {return this->getPieceType() == KING;}

	bool isPieceWhite() {return this->getPieceColor() == WHITE;}
	bool isPieceBlack() {return this->getPieceColor() == BLACK;}
	bool isSameColor(int src, int dest);
	
	virtual bool isLegalMove(int dest);
	virtual bool canCastle(int dest);
	virtual void promotePawn(int dest);
	virtual bool movedIntoCheck(int dest);
	bool causeCheck(int dest);
	// virtual bool isChecked(int src, int dest);

private:
	int square; // position of the piece on the board [0, 63]
	int value; // pawn - 1, knight - 3, bishop - 3, rook - 5, queen - 9, king - infinity (use 10), empty - 0
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)
};

class Pawn : public Piece
{
public:
	virtual ~Pawn(); // destructor
	Pawn(const Pawn & object) = default; // copy constructor
	Pawn & operator =(const Pawn & object) = default; // copy assignment

	// constructor with piece initialization
	Pawn() : Piece() {}

	// constructor with piece initialization
	Pawn(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	virtual void promotePawn(int dest);
	bool isFirstMove();
};	

class Knight : public Piece
{
public:
	virtual ~Knight(); // destructor
	Knight(const Knight & object) = default; // copy constructor
	Knight & operator =(const Knight & object) = default; // copy assignment

	// constructor with piece initialization
	Knight() : Piece() {}

	// constructor with piece initialization
	Knight(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	bool isPinned();

};

class Bishop : public Piece
{
public:
	virtual ~Bishop(); // destructor
	Bishop(const Bishop & object) = default; // copy constructor
	Bishop & operator =(const Bishop & object) = default; // copy assignment

	// constructor with piece initialization
	Bishop() : Piece() {}

	// constructor with piece initialization
	Bishop(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	bool isPinned();
};

class Rook : public Piece
{
public:
	virtual ~Rook(); // destructor
	Rook(const Rook & object) = default; // copy constructor
	Rook & operator =(const Rook & object) = default; // copy assignment

	// constructor with piece initialization
	Rook() : Piece() {}

	// constructor with piece initialization
	Rook(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	bool isFirstMove();
	bool isPinned();
};

class Queen : public Piece
{
public:
	virtual ~Queen(); // destructor
	Queen(const Queen & object) = default; // copy constructor
	Queen & operator =(const Queen & object) = default; // copy assignment

	// constructor with piece initialization
	Queen() : Piece() {}

	// constructor with piece initialization
	Queen(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	bool isPinned();
};

class King : public Piece
{
public:
	virtual ~King(); // destructor
	King(const King & object) = default; // copy constructor
	King & operator =(const King & object) = default; // copy assignment

	// constructor with piece initialization
	King() : Piece() {}

	// constructor with piece initialization
	King(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isLegalMove(int dest);
	bool isFirstMove();
	virtual bool canCastle(int dest);
	// virtual bool isChecked(int src, int dest);
	bool isDoubleChecked();
	int numKingMoves();

	virtual bool movedIntoCheck(int dest);
};

class Empty : public Piece
{
public:
	virtual ~Empty(); // destructor
	Empty(const Empty & object) = default; // copy constructor
	Empty & operator =(const Empty & object) = default; // copy assignment

	// constructor with piece initialization
	Empty() : Piece() {}

	// constructor with piece initialization
	Empty(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}
};


/*****
	GLOBAL FUNCTIONS / OBJECTS
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

extern Chess chess; // global object

#endif // CHESS_H