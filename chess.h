/****************************************************************************************************/  
/*									Title:           chess.h										*/
/*									Author:          Lior Bragilevsky								*/
/*									Related Files:   chess.cpp										*/
/*									Project:         chessCAMO										*/
/*									Version:         1.0											*/
/*									Last Revision:   June 5th, 2020									*/
/****************************************************************************************************/

/*
Project Description:
chessCAMO is a newly proposed chess variant that stands for "Calculations Always Make Opportunities"
and was inspired by the very popular chess variant "Crazyhouse" (https://en.wikipedia.org/wiki/Crazyhouse).
The "CAMO" portion comes from the idea that in this variant, at any turn, a player can sacrifice making a 
move to replace one of their pieces with a piece from the "piece reservoir". This has the affect of 
camouflaging each piece that is currently on the board. The piece reservoir is limited in size and 
cannot be re-stocked with pieces. Once a player decides to use a piece from the reservoir to replace
one of his existing pieces on the board, their turn is over. An obvious rule is that player cannot use
the reservoir while in check as they must make a move to avoid check. Additionally, just like in the 
Crazyhouse variant, a player cannot directly promote a pawn from the reservoir. That is a pawn from 
the reservoir must be placed on the board first and is only allowed to be promoted on the next move.

To Do:
	Regular Chess:
		1) Three move repetition (draw) & 50 move rule.
		2) Board representation undo (use queue?).
		3) Input as PGN rather than two integers (template) - working currently but not really a template.
		4) GUI - nice to be able to move pieces with mouse rather than inputting coordinates.

	ChessCAMO:
		1) Make piece reservoir (stack)
		2) Allow players to replace existing pieces with reservoir pieces
		3) Implement abovementioned check and pawn promotion rules
		4) Make test cases to confirm that overall functionality still works

Why?
	I decided to create this project due to my growing interest in software development. Coming from an
	Engineering background with Deep Learning research experience (Recent Master of Applied Science
	graduate), I realized that writing and developing code (both front and back end) were highly
	appealing to me. Thus, I spent a lot of time building upon my existing coding knowledge through	online
	material and knew that it is time to test my knowledge on a challenging project. Additionally, I played
	chess (not very seriously) consistently since the age of about 4-5, so one can say that it is a very
	big part of my life. As a result, this project allowed me to gain valuable experience and knowledge
	while working on a topic that I am passionate about and have expertise in. 
	Any constructive criticism regarding this project is welcomed and appreciated, as ultimately I love
	learning new things and getting better!       
*/

#ifndef CHESS_H // header guard to prevent multiple includes of the classes (results in compilation error)
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <string>
#include <stdlib.h>     /* abs, system */
#include <algorithm>    /* min, max */
#include <fstream>
#include <windows.h> // for console text colors

using namespace std;

// some colors for console text
#define GREEN 10
#define CYAN 11
#define RED 12
#define PINK 13
#define YELLOW 14
#define DEFAULT 15

enum pieceType {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, EMPTY};
enum pieceColor {BLACK, NEUTRAL, WHITE}; // neutral corresponds to an empty square

// forward declaration
class Piece; 

/*************************************************************************************/
/*								CHESS CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Chess
{
public:
	/************************************* Big Three ********************************/
	~Chess(); // destructor
	Chess(const Chess & object) = default; // copy constructor
	Chess & operator =(const Chess & object) = default; // copy assignment

	// default constructor with default piece initialization
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

/*************************************************************************************/
/*								PIECE CLASS - HELPER FUNCTIONS						 */
/*************************************************************************************/
	// Decide if it is an attacking move or regular move
	void makeMoveForType(int src, int dest);
	void pieceSwap(int src, int dest, vector<Piece*> & board);
	void handleChangeTurn();
	void handleCheckmate();
	void handleStalemate();

	bool undoMove(int src, int dest, Piece* king, Piece* piece, Piece* undo_piece, bool undo_moveInfo, string check_type);
	bool singleCheckPieceIterator(Piece* piece, Piece* king); // for isCheckmate (single)
	bool doubleCheckPieceIterator(Piece* king); // for isCheckmate (double) - only king movement matters

	pieceColor switchTurn();
};

/*************************************************************************************/
/*								PIECE CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Piece : public Chess
{
public:
	/************************************* Big Three ********************************/
	~Piece() = default; // destructor
	Piece(const Piece & object) = default; // copy constructor
	Piece & operator =(const Piece & object) = default; // copy assignment

	// default constructor with default piece initialization
	Piece() : Chess(), square{0}, type{EMPTY}, color{NEUTRAL}, moved{false} {}

	// constructor with valid piece information initialization
	Piece(int square, pieceType type, pieceColor color) : moved{false}
	{this->square = square; this->type = type; this->color = color;}

	// Mutator and accessor functions for determining/setting the piece value of an object
	int getPieceSquare() const {return square;}
	void setPieceSquare(int square) {this->square = square;}

	// Mutator and accessor functions for determining/setting the piece type of an object
	pieceType getPieceType() const {return type;}
	void setPieceType(pieceType type) {this->type = type;}

	// Mutator and accessor functions for determining/setting the piece color of an object
	pieceColor getPieceColor() const {return color;}
	void setPieceColor(pieceColor color) {this->color = color;}

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

	bool isLegalMove(int dest);
	bool causeCheck(int dest);
	bool causeDoubleCheck(int dest);
	bool isPinned(int dest);
	bool isPathFree(int dest);
	
	virtual bool isPossibleMove(int dest);
	virtual bool canCastle(int dest);
	virtual void promotePawn(int dest);
	virtual bool movedIntoCheck(int dest);
	virtual void enPassantHandling(int src);

private:
	int square; // position of the piece on the board [0, 63]
	pieceType type;
	pieceColor color;
	bool moved; // has the piece been moved yet (important for pawns and castling)
};

/*************************************************************************************/
/*								PAWN CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Pawn : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Pawn() = default; // destructor
	Pawn(const Pawn & object) = default; // copy constructor
	Pawn & operator =(const Pawn & object) = default; // copy assignment

	// default constructor with default piece initialization
	Pawn() : Piece(), en_passant{false} {}

	// constructor with valid piece information initialization
	Pawn(int square, pieceType type, pieceColor color) : Piece(square, type, color), en_passant{false} {}

	// Mutator and accessor functions for determining/setting a pawn's en-passant abilities 
	virtual bool getEnPassant() const {return en_passant;}
	virtual void setEnPassant(bool en_passant) {this->en_passant = en_passant;}

	virtual bool isPossibleMove(int dest);
	virtual void enPassantHandling(int src);
	virtual void promotePawn(int dest);

private:
	bool en_passant;
};	

/*************************************************************************************/
/*								KNIGHT CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Knight : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Knight() = default; // destructor
	Knight(const Knight & object) = default; // copy constructor
	Knight & operator =(const Knight & object) = default; // copy assignment

	// default constructor with default piece initialization
	Knight() : Piece() {}

	// constructor with valid piece information initialization
	Knight(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								BISHOP CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Bishop : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Bishop() = default; // destructor
	Bishop(const Bishop & object) = default; // copy constructor
	Bishop & operator =(const Bishop & object) = default; // copy assignment

	// default constructor with default piece initialization
	Bishop() : Piece() {}

	// constructor with valid piece information initialization
	Bishop(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								ROOK CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Rook : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Rook() = default; // destructor
	Rook(const Rook & object) = default; // copy constructor
	Rook & operator =(const Rook & object) = default; // copy assignment

	// default constructor with default piece initialization
	Rook() : Piece() {}

	// constructor with valid piece information initialization
	Rook(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								QUEEN CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Queen : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Queen() = default; // destructor
	Queen(const Queen & object) = default; // copy constructor
	Queen & operator =(const Queen & object) = default; // copy assignment

	// default constructor with default piece initialization
	Queen() : Piece() {}

	// constructor with valid piece information initialization
	Queen(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								KING CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class King : public Piece
{
public:
	/************************************* Big Three ********************************/
	~King() = default; // destructor
	King(const King & object) = default; // copy constructor
	King & operator =(const King & object) = default; // copy assignment

	// default constructor with default piece initialization
	King() : Piece() {}

	// constructor with valid piece information initialization
	King(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

	virtual bool isPossibleMove(int dest);
	virtual bool canCastle(int dest);
	virtual bool movedIntoCheck(int dest);
};

/*************************************************************************************/
/*								EMPTY CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Empty : public Piece
{
public:
	/************************************* Big Three ********************************/
	~Empty() = default; // destructor
	Empty(const Empty & object) = default; // copy constructor
	Empty & operator =(const Empty & object) = default; // copy assignment

	// default constructor with default piece initialization
	Empty() : Piece() {}

	// constructor with valid piece information initialization
	Empty(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}
};

/*************************************************************************************/
/*								GLOBAL FUNCTIONS / OBJECTS							 */
/*************************************************************************************/
namespace chessCAMO
{
	// Description:    	Places the pieces on the board at their correct starting positions
	// Pre-condition:  	'chess'		 - global object is initialized
	//				   	'board_size'  - 8x8 board has 64 index positions
	// Post-condition: 	Instantiates new objects corresponding to the pieces, places them
	//				   	in the corresponding index of the board vector and set the global
	//				   	object's board variable  
	void boardInit(int board_size = 64); // Board intialization

	// Description:    	Iterates through the pieces on a current board representation to 
	// 				   	produce the board on the console screen
	// Pre-condition:  	'board'		 - a board is created and contains pieces
	// Post-condition: 	Each piece of the current board representation is printed to the
	//					screen using a corresponding letter inside a formatted board
	void printBoard(const vector<Piece*> & board); // Print the current board position

	// Description:    	At any moment, the players can either continue, draw, or resign
	// Pre-condition:  	None
	// Post-condition: 	Depending on the users choice, the program either continues
	//					('y' || 'd' + 'n') or terminates ('d' + 'y' || 'r')
	void drawOrResign(); // resign or draw

	// Description:    	Prints the given message ('text') with a given 'color' to console
	// Pre-condition:  	'text'		- message is created
	//					'color'		- one of the defined values at the top of the file
	// Post-condition: 	The message is printed to the screen with color chosen and then
	//					the color is changed back to default prior to return
	void printMessage(string text, int color);
}

extern Chess chess; // global object (used to avoid passing it into most member functions)

#endif // CHESS_H