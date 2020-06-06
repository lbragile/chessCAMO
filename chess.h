/****************************************************************************************************/
/*									Title:           chess.h										*/
/*									Author:          Lior Bragilevsky								*/
/*									Related Files:   chess.cpp										*/
/*									Project:         chessCAMO										*/
/*									Version:         1.0											*/
/*									Last Revision:   June 6th, 2020									*/
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
	/*********************************** BIG THREE *********************************/
	~Chess(); // destructor
	Chess(const Chess & object) = default; // copy constructor
	Chess & operator =(const Chess & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default board parameter initialization
	Chess() : checkmate{false}, stalemate{false}, check{false}, double_check{false}, turn{WHITE} {} // intentionally blank

	/************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
	// Current board representation/state
	vector<Piece*> getBoard() const {return board;}
	void setBoard(const vector<Piece*> & board) {this->board = board;}

	// Checking pieces and checked king
	stack<Piece*> getCheckStack() const {return checkStack;}
	void setCheckStack(const stack<Piece*> & checkStack) {this->checkStack = checkStack;}

	// Checking state (single check)
	bool getCheck() const {return check;}
	void setCheck(bool check) {this->check = check;}

	// Checking state (double check)
	bool getDoubleCheck() const {return double_check;}
	void setDoubleCheck(bool double_check) {this->double_check = double_check;}

	// Checkmate state
	bool getCheckmate() const {return checkmate;}
	void setCheckmate(bool checkmate) {this->checkmate = checkmate;}

	// Stalemate state
	bool getStalemate() const {return stalemate;}
	void setStalemate(bool stalemate) {this->stalemate = stalemate;}

	// Player's turn information
	pieceColor getTurn() const {return turn;}
	void setTurn(pieceColor turn) {this->turn = turn;} // useful when moving a piece

	// En-passant ability information 
	virtual bool getEnPassant() const {return this->getEnPassant();}
	virtual void setEnPassant(bool en_passant) {} // purposely left definition blank
	/************************************* END *************************************/

	// Description:    	Moves a piece on the board from 'src' to 'dest' if conditions
	//					for a legal move are met
	// Pre-condition:  	'chess'		- object is created
	//					'src' 		- source square (piece's current location)
	//					'dest'		- destination square (piece's ending location)
	// Post-condition: 	The pieces at 'src' and 'dest' positions are swapped.
	//					If needed (attacking, castling, etc.) an empty square is made.
	//					The board's state is updated to indicate that the move occured.
	//					On failure, an error message is printed and user is asked to retry.
	void makeMove(int src, int dest); // for src = "52", dest = "36" type input (coordinate numbers)

	// same as above, but converts the string into it's coordinate (integer) and called above function.
	void makeMove(string src, string dest); // overloaded for src = "e2", dest = "e4" type inputs

	// Description:    	Decide if a move caused a checkmate
	// Pre-condition:  	'chess'		- object is created
	//					'check_type'	- string representing the checking type ("single" or "double")		
	// Post-condition:	If board's state is in checkmate, calls handleCheckmate() to print messages
	// 					to console indicating the winner. Else, game continues as usual.
	void isCheckmate(string check_type);

	// Description:    	Decide if a move caused a stalemate
	// Pre-condition:  	'chess'		- object is created		
	// Post-condition:	If board's state is in stalemate, calls handleStalemate() to print messages
	// 					to console indicating that game is drawn. Else, game continues as usual.
	bool isStalemate();
	
private:
	vector<Piece*> board; 		// Chess board representation with the pieces in correct spots
	stack<Piece*> checkStack; 	// Stores the pieces involved in a checking scenario
	bool checkmate;				/* \  								*/
	bool stalemate;				/*  Store relevant information for  */
	bool check;					/*  game continuation decisions	  	*/
	bool double_check;			/* /  								*/
	pieceColor turn;			// Either WHITE or BLACK, note NEUTRAL is ignored here 

/*************************************************************************************/
/*								PIECE CLASS - HELPER FUNCTIONS						 */
/*************************************************************************************/
	// Description:    	A move can be one of: attack, castle, en-passant, regular
	// Pre-condition:  	'chess'		- object is created
	//					'src'		- valid input source square
	//					'dest'		- valid input destination square (move is legal)		
	// Post-condition:	Swaps the pieces on the board according to 'src' and 'dest' and proper
	//					chess rules, using pieceSwap(.). If a new empty square must be created,
	// 					this is handled. Returns board representation with the made move.
	void makeMoveForType(int src, int dest);

	// Description:    	Used in makeMoveForType(.) to swap pieces on the board
	// Pre-condition:  	'chess'		- object is created
	//					'src'		- valid input source square
	//					'dest'		- valid input destination square (move is legal)
	//					'board'		- valid board representation		
	// Post-condition:	Swaps the pieces on the board according to 'src' and 'dest'.
	void pieceSwap(int src, int dest, vector<Piece*> & board);

	// Description:    	Indicates who will move next via a message to console
	// Pre-condition:  	'chess'		- object is created	
	// Post-condition:	Player turn is switched, board is printed, and message is displayed
	//					if game is not over to indicate whose turn it is.
	void handleChangeTurn();

	// Description:    	Indicates which player won by checkmate via a message to console
	// Pre-condition:  	'chess'		- object is created
	//					A move was made (cannot checkmate in less than 2 moves in theory)	
	// Post-condition:	global object's checkmate state is set to true (to end the algorithm)
	void handleCheckmate();

	// Description:    	Indicates the game is drawn via a message to console
	// Pre-condition:  	'chess'		- object is created
	//					A move was made
	// Post-condition:	global object's stalemate state is set to true (to end the algorithm)
	void handleStalemate();

	// Description:    	After a move is made, can undo it if move was invalid and return to
	//					previous board state
	// Pre-condition:  	'chess'			- object is created
	//					'src' 			- source square of piece prior to current board state
	//					'dest' 			- destination square of piece prior to current board state
	//					'king'			- king that is being attacked currently
	//					'piece'			- piece that is attacking the king currently
	//					'undo_piece' 	- if move fails, this is the piece that was moved previously
	//					'undo_moveInfo'	- if move fails, this is the piece's move information
	//					'check_type'	- "single" or "double" check
	// Post-condition:	If after move, the 'king' is still in check (single or double) or the move was
	//					invalid, output warning message and undo the move. Else, continue the game
	//					without undoing the move.
	bool undoMove(int src, int dest, Piece* king, Piece* piece, Piece* undo_piece, bool undo_moveInfo, string check_type);
	
	// Description:    	If in a single check, see if piece can defend the king, capture attacking piece,
	//					or move the king out of check. Used in isCheckmate("single")
	// Pre-condition:  	'chess'			- object is created
	//					'king'			- king that is being attacked currently
	//					'piece'			- piece that is attacking the king currently
	// Post-condition:	If no legal moves found return true (checkmate), else return false and make the move
	bool singleCheckPieceIterator(Piece* piece, Piece* king); 
	
	// Description:    	If in a double check, see if the king can move out of check as this is the only
	//					valid move option. Used in isCheckmate("double")
	// Pre-condition:  	'chess'			- object is created
	//					'king'			- king that is being attacked currently
	// Post-condition:	If no legal moves found return true (checkmate), else return false and make the move
	bool doubleCheckPieceIterator(Piece* king);

	// Description:    	Decides whose turn it is currently and updates the private member variable accordingly
	// Pre-condition:  	'chess'			- object is created
	// Post-condition:	'chess.turn' is set to the correct player
	pieceColor switchTurn();
};

/*************************************************************************************/
/*								PIECE CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Piece : public Chess
{
public:
	/*********************************** BIG THREE *********************************/
	~Piece() = default; // destructor
	Piece(const Piece & object) = default; // copy constructor
	Piece & operator =(const Piece & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Piece() : Chess(), square{0}, moved{false}, type{EMPTY}, color{NEUTRAL} {} // intentionally blank

	// constructor with valid piece information initialization
	Piece(int square, pieceType type, pieceColor color) : moved{false}
	{this->square = square; this->type = type; this->color = color;}

	/************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
	// Piece square information
	int getPieceSquare() const {return square;}
	void setPieceSquare(int square) {this->square = square;}

	// Piece type information
	pieceType getPieceType() const {return type;}
	void setPieceType(pieceType type) {this->type = type;}

	// Piece color information
	pieceColor getPieceColor() const {return color;}
	void setPieceColor(pieceColor color) {this->color = color;}

	// Piece moving state information(useful for pawns, rooks, kings)
	bool getPieceMoveInfo() const {return moved;}
	void setPieceMoveInfo(bool moved) {this->moved = moved;}
	/************************************* END *************************************/

	/************************ TYPE DETERMINATION FUNCTIONS *************************/
	bool isEmpty() {return this->getPieceType() == EMPTY;}
	bool isPawn() {return this->getPieceType() == PAWN;}
	bool isKnight() {return this->getPieceType() == KNIGHT;}
	bool isBishop() {return this->getPieceType() == BISHOP;}
	bool isRook() {return this->getPieceType() == ROOK;}
	bool isQueen() {return this->getPieceType() == QUEEN;}
	bool isKing() {return this->getPieceType() == KING;}
	/************************************* END *************************************/

	/************************ COLOR DETERMINATION FUNCTIONS ************************/
	bool isPieceWhite() {return this->getPieceColor() == WHITE;}
	bool isPieceBlack() {return this->getPieceColor() == BLACK;}
	/************************************* END *************************************/

	// legal move determination functions
	bool isSameColor(int dest);
	bool isPinned(int dest);
	bool isPathFree(int dest);
	bool isLegalMove(int dest);

	// check determination functions - see if game ended or can continue (calls checkmate/stalemate)
	bool causeCheck(int dest);
	bool causeDoubleCheck(int dest);
	
	/****************************** VIRTUAL FUNCTIONS ******************************/
	virtual bool isPossibleMove(int dest);
	virtual bool canCastle(int dest);
	virtual void promotePawn(int dest);
	virtual bool movedIntoCheck(int dest);
	virtual void enPassantHandling(int src);
	/************************************* END *************************************/

private:
	int square; 		// position of the piece on the board [0, 63] -> [top left, bottom right]
	bool moved; 		// has the piece been moved yet?
	pieceType type;		// Pawn, Knight, Bishop, Rook, Queen, King, or Empty
	pieceColor color;	// Black, Neutral, or White
};

/*************************************************************************************/
/*								PAWN CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Pawn : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~Pawn() = default; // destructor
	Pawn(const Pawn & object) = default; // copy constructor
	Pawn & operator =(const Pawn & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Pawn() : Piece(), en_passant{false} {} // intentionally blank

	// constructor with valid piece information initialization
	Pawn(int square, pieceType type, pieceColor color) : Piece(square, type, color), en_passant{false} {} // intentionally blank

	/************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
	// En-passant ability information 
	virtual bool getEnPassant() const {return en_passant;}
	virtual void setEnPassant(bool en_passant) {this->en_passant = en_passant;}
	/************************************* END *************************************/

	virtual bool isPossibleMove(int dest);
	virtual void enPassantHandling(int src);
	virtual void promotePawn(int dest);

private:
	bool en_passant;	// Can this pawn en-passant it's rival currently?
};	

/*************************************************************************************/
/*								KNIGHT CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Knight : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~Knight() = default; // destructor
	Knight(const Knight & object) = default; // copy constructor
	Knight & operator =(const Knight & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Knight() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	Knight(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								BISHOP CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Bishop : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~Bishop() = default; // destructor
	Bishop(const Bishop & object) = default; // copy constructor
	Bishop & operator =(const Bishop & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Bishop() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	Bishop(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								ROOK CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Rook : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~Rook() = default; // destructor
	Rook(const Rook & object) = default; // copy constructor
	Rook & operator =(const Rook & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Rook() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	Rook(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								QUEEN CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class Queen : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~Queen() = default; // destructor
	Queen(const Queen & object) = default; // copy constructor
	Queen & operator =(const Queen & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Queen() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	Queen(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank

	virtual bool isPossibleMove(int dest);
};

/*************************************************************************************/
/*								KING CLASS - MEMBER FUNCTIONS						 */
/*************************************************************************************/
class King : public Piece
{
public:
	/*********************************** BIG THREE *********************************/
	~King() = default; // destructor
	King(const King & object) = default; // copy constructor
	King & operator =(const King & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	King() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	King(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank

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
	/*********************************** BIG THREE *********************************/
	~Empty() = default; // destructor
	Empty(const Empty & object) = default; // copy constructor
	Empty & operator =(const Empty & object) = default; // copy assignment
	/************************************* END *************************************/

	// default constructor with default piece initialization
	Empty() : Piece() {} // intentionally blank

	// constructor with valid piece information initialization
	Empty(int square, pieceType type, pieceColor color) : Piece(square, type, color) {} // intentionally blank
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

extern Chess chess; // global object (used to avoid passing it into many member functions)

#endif // CHESS_H