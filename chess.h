#ifndef CHESS_H
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <stdlib.h>     /* abs */
#include <algorithm>    /* min, max */
#include <fstream>
#include <windows.h>
#include <string>

// some colors for console text
#define GREEN 10
#define CYAN 11
#define RED 12
#define PINK 13
#define YELLOW 14
#define DEFAULT 15

using namespace std;

enum pieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY}; // caps to avoid class name conflict
enum pieceColor {BLACK, NEUTRAL, WHITE};

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
	Chess() : checkmate{false}, stalemate{false}, check{false}, double_check{false}, turn{WHITE} {}

	// Mutator and accessor functions for determining/setting the board's current state
	vector<Piece*> getBoard() const {return board;}
	void setBoard(const vector<Piece*> & board) {this->board = board;}

	// Mutator and accessor functions for determining/setting the board's checking piece/king
	stack<Piece*> getCheckStack() const {return checkStack;}
	void setCheckStack(const stack<Piece*> & checkStack) {this->checkStack = checkStack;}

	// Mutator and accessor functions for determining/setting the board's checking state 
	bool getCheck() const {return check;}
	void setCheck(bool check) {this->check = check;}
	bool getDoubleCheck() const {return double_check;}
	void setDoubleCheck(bool double_check) {this->double_check = double_check;}

	// Mutator and accessor functions for determining/setting the board's checkmate state
	bool getCheckmate() const {return checkmate;}
	void setCheckmate(bool checkmate) {this->checkmate = checkmate;}

	// Mutator and accessor functions for determining/setting the board's stalemate state
	bool getStalemate() const {return stalemate;}
	void setStalemate(bool stalemate) {this->stalemate = stalemate;}

	// Mutator and accessor functions for determining/setting the player's turn
	pieceColor getTurn() const {return turn;}
	void setTurn(pieceColor turn) {this->turn = turn;} // useful when moving a piece

	// Mutator and accessor functions for determining/setting a pawn's en-passant abilities 
	virtual bool getEnPassant() const {return this->getEnPassant();}
	virtual void setEnPassant(bool en_passant) {}

	void makeMove(int src, int dest); // for src = "52", dest = "36" type input (coordinate numbers)
	void makeMove(string src, string dest); // overloaded for src = "e2", dest = "e4" type inputs

	void isCheckmate(string checkType);
	bool isStalemate();
	
private:
	vector<Piece*> board; // overall board state
	stack<Piece*> checkStack; // needed to determine if a given player's king is in check
	bool checkmate;
	bool stalemate;
	bool check;
	bool double_check;
	pieceColor turn;

	// Decide if it is an attacking move or regular move
	void makeMoveForType(int src, int dest);
	void handleChangeTurn();
	void handleCheckmate();
	void handleStalemate();

	bool undoMove(int src, int dest, Piece* king, Piece* piece, Piece* undo_piece, bool undo_moveInfo, string check_type);
	bool singleCheckPieceIterator(int src, int dest); // for isCheckmate (single)
	bool doubleCheckPieceIterator(int dest); // for isCheckmate (double) - only king movement matters

protected:
	bool pathIterator(int src, int dest, int increment); // for isPathFree
	bool destInPath(int src, int dest, int pin);
	int squareOfPieceInPath(int src, int dest);
	int incrementChoice(int src, int dest);

};

class Piece : public Chess
{
public:
	~Piece() = default; // destructor
	Piece(const Piece & object) = default; // copy constructor
	Piece & operator =(const Piece & object) = default; // copy assignment

	// constructor with piece initialization
	Piece() : Chess(), square{0}, value{0}, type{EMPTY}, color{NEUTRAL}, moved{false} {}

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
	bool isSameColor(int dest);
	
	virtual bool isPossibleMove(int dest);
	virtual bool canCastle(int dest);
	virtual void promotePawn(int dest);
	virtual bool movedIntoCheck(int dest);
	virtual void enPassantHandling(int src);

	bool causeCheck(int dest);
	bool causeDoubleCheck(int dest);
	bool isPinned(int dest);
	bool isPathFree(int dest);

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
	~Pawn() = default; // destructor
	Pawn(const Pawn & object) = default; // copy constructor
	Pawn & operator =(const Pawn & object) = default; // copy assignment

	// constructor with piece initialization
	Pawn() : Piece(), en_passant{false} {}

	// constructor with piece initialization
	Pawn(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color), en_passant{false} {}

	// Mutator and accessor functions for determining/setting a pawn's en-passant abilities 
	virtual bool getEnPassant() const {return en_passant;}
	virtual void setEnPassant(bool en_passant) {this->en_passant = en_passant;}

	virtual bool isPossibleMove(int dest);
	virtual void enPassantHandling(int src);
	virtual void promotePawn(int dest);

private:
	bool en_passant;
};	

class Knight : public Piece
{
public:
	~Knight() = default; // destructor
	Knight(const Knight & object) = default; // copy constructor
	Knight & operator =(const Knight & object) = default; // copy assignment

	// constructor with piece initialization
	Knight() : Piece() {}

	// constructor with piece initialization
	Knight(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isPossibleMove(int dest);
};

class Bishop : public Piece
{
public:
	~Bishop() = default; // destructor
	Bishop(const Bishop & object) = default; // copy constructor
	Bishop & operator =(const Bishop & object) = default; // copy assignment

	// constructor with piece initialization
	Bishop() : Piece() {}

	// constructor with piece initialization
	Bishop(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isPossibleMove(int dest);
};

class Rook : public Piece
{
public:
	~Rook() = default; // destructor
	Rook(const Rook & object) = default; // copy constructor
	Rook & operator =(const Rook & object) = default; // copy assignment

	// constructor with piece initialization
	Rook() : Piece() {}

	// constructor with piece initialization
	Rook(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isPossibleMove(int dest);
};

class Queen : public Piece
{
public:
	~Queen() = default; // destructor
	Queen(const Queen & object) = default; // copy constructor
	Queen & operator =(const Queen & object) = default; // copy assignment

	// constructor with piece initialization
	Queen() : Piece() {}

	// constructor with piece initialization
	Queen(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	virtual bool isPossibleMove(int dest);
};

class King : public Piece
{
public:
	~King() = default; // destructor
	King(const King & object) = default; // copy constructor
	King & operator =(const King & object) = default; // copy assignment

	// constructor with piece initialization
	King() : Piece() {}

	// constructor with piece initialization
	King(int square, int value, pieceType type, pieceColor color) : Piece(square, value, type, color) {}

	bool isDoubleChecked();
	int numKingMoves();

	virtual bool isPossibleMove(int dest);
	virtual bool canCastle(int dest);
	virtual bool movedIntoCheck(int dest);
};

class Empty : public Piece
{
public:
	~Empty() = default; // destructor
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