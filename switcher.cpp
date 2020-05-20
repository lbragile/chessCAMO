#include "switcher.h"
#include <iomanip>
#include <vector>

using namespace std;

// // copy assignment
// Switcher & Switcher::operator =(const Switcher & object)
// {
// 	this->square = object.square;
// 	this->value = object.value;
// 	this->type = object.type;
// 	this->color = object.color;
// 	this->moved = object.moved;

// 	return *this;
// }

// // move assignment
// Switcher  &Switcher::operator =(Switcher && object)
// {
// 	*this = std::move(object);
// 	return *this;
// }

// default constructor
Switcher::Switcher() : square{0}, value{0}, type{Empty}, color{Black}, moved{false}
{
 // intentionally blank
}

// constructor with piece initialization
Switcher::Switcher(unsigned int square, unsigned int value, pieceType type, pieceColor color) : moved{false}
{
	this->square = square;
	this->value = value;
	this->type = type;
	this->color = color;
}

// Mutator and accessor functions for determining/setting the piece type of an object
pieceType Switcher::getPieceType() const
{
	return type;
}

void Switcher::setPieceType(pieceType type)
{
	this->type = type;
}

// Mutator and accessor functions for determining/setting the piece color of an object
pieceColor Switcher::getPieceColor() const
{
	return color;
}

void Switcher::setPieceColor(pieceColor color)
{
	this->color = color;
}

// Mutator and accessor functions for determining/setting the piece value of an object
unsigned int Switcher::getPieceValue() const
{
	return value;
}

void Switcher::setPieceValue(unsigned int value)
{
	this->value = value;
}

// Mutator and accessor functions for determining/setting the piece square of an object
unsigned int Switcher::getPieceSquare() const
{
	return square;
}

void Switcher::setPieceSquare(unsigned int square)
{
	this->square = square;
}

// Mutator and accessor functions for determining/setting the moving state of an object
unsigned int Switcher::getPieceMovevement() const
{
	return moved;
}

void Switcher::setPieceMovement(bool movement)
{
	moved = movement;
}


// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
// Return 'true' if move is valid, 'false' otherwise
bool Switcher::isValidMove(int src, int dest, vector<Switcher> & board) const
{
	/* TODO: 
		1. add attacking move for pawn
		2. add piece barrier conditions
		3. castling
		4. move into square with a piece of opponent (attack)
		5. Promotion?
	*/

	int src_row = src / 8, src_col = src % 8;
	int dest_row = dest / 8, dest_col = dest % 8;

	bool valid;
	int diff = (dest - src) > 0 ? dest - src : src - dest;

	switch(this->getPieceType())
	{
		case Rook: // moves in same row and column
			valid = (src_row == dest_row || src_col == dest_col) && diff != 0 && pathEmptyRook(src, dest, board);
			break;

		case Knight: // most complicated piece (can jump over pieces also)
			//  8 possible moves - middle of board with 2 squares from edge minimum
			if(src_row > 1 && src_row < 6 && src_col > 1 && src_col < 6) 
				valid = (diff == 6 || diff == 10 || diff == 15 || diff == 17);

			// 6 possible moves - row 1
			else if(src_row == 1 && src_col > 1 && src_col < 6)
				valid = (diff == 6 || diff == 10 || dest - src == 15 || dest - src == 17);

			// 6 possible moves - row 6
			else if(src_row == 6 && src_col > 1 && src_col < 6)
				valid = (diff == 6 || diff == 10 || src-dest == 15 || src-dest == 17);

			// 6 possible moves - col 1
			else if(src_col == 1 && src_row > 1 && src_row < 6)
				valid = (src-dest == 6 || dest - src == 10 || diff == 15 || diff == 17);

			// 6 possible moves - col 6
			else if(src_col == 6 && src_row > 1 && src_row < 6)
				valid = (dest - src == 6 || src-dest == 10 || diff == 15 || diff == 17);

			// 4 possible moves - row 0
			else if(src_row == 0 && src_col > 1 && src_col < 6)
				valid = (dest - src == 6 || dest - src == 10 || dest - src == 15 || dest - src == 17);

			// 4 possible moves - row 7
			else if(src_row == 7 && src_col > 1 && src_col < 6)
				valid = (src-dest == 6 || src-dest == 10 || src-dest == 15 || src-dest == 17);

			// 4 possible moves - col 0
			else if(src_col == 0 && src_row > 1 && src_row < 6)
				valid = (src-dest == 6 || dest - src == 10 || src-dest == 15 || dest - src == 17);

			// 4 possible moves - col 7
			else if(src_col == 7 && src_row > 1 && src_row < 6)
				valid = (src-dest == 6 || src-dest == 10 || src-dest == 15 || src-dest == 17);

			// 4 possible moves - col 1, row 1
			else if(src_col == 1 && src_row == 1)
				valid = (src-dest == 6 || dest - src == 10 || dest - src == 15 || dest - src == 17);

			// 4 possible moves - col 6, row 1
			else if(src_col == 6 && src_row == 1)
				valid = (dest - src == 6 || src-dest == 10 || dest - src == 15 || dest - src == 17);

			// 4 possible moves - col 1, row 6
			else if(src_col == 1 && src_row == 6)
				valid = (src-dest == 6 || dest - src == 10 || src-dest == 15 || src-dest == 17);

			// 4 possible moves - col 6, row 6
			else if(src_col == 6 && src_row == 6)
				valid = (dest - src == 6 || src-dest == 10 || src-dest == 15 || src-dest == 17);

			// 3 possible moves - col 0, row 1
			else if(src_col == 0 && src_row == 1)
				valid = (src-dest == 6 || dest - src == 10 || dest - src == 17);

			// 3 possible moves - col 7, row 1
			else if(src_col == 7 && src_row == 1)
				valid = (dest - src == 6 || src-dest == 10 || dest - src == 15);

			// 3 possible moves - col 0, row 6
			else if(src_col == 0 && src_row == 6)
				valid = (src-dest == 6 || dest - src == 10 || src-dest == 15);

			// 3 possible moves - col 7, row 6
			else if(src_col == 7 && src_row == 6)
				valid = (dest - src == 6 || src-dest == 10 || src-dest == 17);

			// 3 possible moves - col 1, row 0
			else if(src_col == 1 && src_row == 0)
				valid = (dest - src == 10 || dest - src == 15 || dest - src == 17);

			// 3 possible moves - col 6, row 0
			else if(src_col == 6 && src_row == 0)
				valid = (dest - src == 6 || dest - src == 15 || dest - src == 17);

			// 3 possible moves - col 1, row 7
			else if(src_col == 1 && src_row == 7)
				valid = (src-dest == 6 || src-dest == 15 || src-dest == 17);

			// 3 possible moves - col 6, row 7
			else if(src_col == 6 && src_row == 7)
				valid = (src-dest == 10 || src-dest == 15 || src-dest == 17);

			// 2 possible moves - col 0, row 0
			else if(src_col == 0 && src_row == 0)
				valid = (dest - src == 10 || dest - src == 17);

			// 2 possible moves - col 0, row 7
			else if(src_col == 0 && src_row == 7)
				valid = (dest - src == 6 || dest - src == 15);

			// 2 possible moves - col 7, row 0
			else if(src_col == 7 && src_row == 0)
				valid = (src-dest == 6 || src-dest == 15);

			// 2 possible moves - col 7, row 7
			else if(src_col == 7 && src_row == 7)
				valid = (src-dest == 10 || src-dest == 17);

			valid = valid && diff != 0;
			break;

		case Bishop:
			valid = (diff % 7 == 0 || diff % 9 == 0) && diff != 0 && pathEmptyBishop(src, dest, board);
			break;

		case King: // like rook and bishop but with end square being 1 away
			valid = (src_row == dest_row || src_col == dest_col || diff % 7 == 0 || diff % 9 == 0) && diff == 1;
			break;

		case Queen: // like rook and bishop
			valid = (diff % 7 == 0 || diff % 9 == 0 || src_row == dest_row || src_col == dest_col) && diff != 0 &&
					 (pathEmptyRook(src, dest, board) || pathEmptyBishop(src, dest, board));
			break;

		case Pawn: // on attack, it can move sideways & first move can be 2 squares forward
			if(this->getPieceMovevement())
			{
				if(this->getPieceColor() == White) // goes up
					valid = src-dest == 8 || ((src-dest == 7 || src-dest == 9) && board[dest].getPieceType() != Empty);
				else // black, goes down
					valid = dest-src == 8 || ((dest-src == 7 || dest-src == 9) && board[dest].getPieceType() != Empty);
			}
			else
			{	
				if(this->getPieceColor() == White) // goes up
					valid = src-dest == 8 || src-dest == 16 || ((src-dest == 7 || src-dest == 9) && board[dest].getPieceType() != Empty);
				else // black, goes down
					valid = dest-src == 8 || dest-src == 16 || ((dest-src == 7 || dest-src == 9) && board[dest].getPieceType() != Empty);
			}

			valid = valid && 1;
			break;

		default:
			valid = false;
	}
    
    return valid;
}

// if move is valid, make the move
// On return, the piece's square value is updated
bool Switcher::makeMove(vector<Switcher> & board, int dest)
{
	/* TODO:
		For pawns this condition must be stricter
		Likewise for castling
	*/
	if(this->isValidMove(this->getPieceSquare(), dest, board))
	{
		std::swap(*this, board[dest]);
		this->setPieceMovement(true); // note that the piece moved (important for castling and pawn's first move)
		return true;
	}
	
	return false;
}

// given a path (src -> dest) determine if there are peices in that path
// returns "true" if path has only empty squares, else "false"
bool Switcher::pathEmptyRook(int src, int dest, vector<Switcher> & board) const
{
	bool empty = false;

	// path doesn't care which way you go
	if(src > dest)
		std::swap(src, dest);

	int src_row = src / 8, src_col = src % 8;
	int dest_row = dest / 8, dest_col = dest % 8;
	int diff = dest-src;

	if(diff == 0) // same square so nothing can be between
		return true;
	else if(diff == 1 || diff == 8) // 1 square apart, must make sure not same color as attack can be between differing colors
		return !(board[src].getPieceColor() == board[dest].getPieceColor());

	while(src < dest-8)
	{
		if(src_row == dest_row)
			src++;
		else if(src_col == dest_col)
			src += 8;
		else
			break;

		empty = board[src].getPieceType() == Empty;
	}
	return empty;
}

bool Switcher::pathEmptyBishop(int src, int dest, vector<Switcher> & board) const
{
	bool empty = false;

	// path doesn't care which way you go
	if(src > dest)
		std::swap(src, dest);

	int diff = dest-src;

	if(diff == 0) // same square so nothing can be between
		return true;
	else if(diff == 7 || diff == 9) // 1 square apart, must make sure not same color as attack can be between differing colors
		return !(board[src].getPieceColor() == board[dest].getPieceColor());

	while(src < dest-10)
	{
		if(diff % 9 == 0)
			src += 9;
		else if(diff % 7 == 0)
			src += 7;

		empty = board[src].getPieceType() == Empty;
	}
	return empty;
}


// bool Switcher::pathEmptyPawn(int src, int dest, vector<Switcher> & board) const
// {
// 	// path doesn't care which way you go
// 	if(src > dest)
// 		std::swap(src, dest);

// 	int src_row = src / 8, src_col = src % 8;
// 	int dest_row = dest / 8, dest_col = dest % 8;

// 	while(src < dest)
// 	{
// 		if(src_row == dest_row)
// 			src++;
// 		else
// 			src += 8;

// 		if(board[src].getPieceType() != Empty)
// 			return true;
// 	}
// 	return false;
// }

// Board intialization
vector<Switcher> initBoard(unsigned int BOARD_SIZE)
{
	Switcher piece;

	// 8x8 board stored as vector of vectors, each row contains the piece objects
	vector<Switcher> board; 

	// initialize the board
	for(unsigned int i = 0; i < BOARD_SIZE; i++)
	{
		if(i < BOARD_SIZE/4) // black
		{
			piece.setPieceSquare(i);
			if(i == 0 || i == 7) // rook
			{
				piece.setPieceValue(5);
				piece.setPieceType(Rook);
			}
			else if(i == 1 || i == 6) // knight
			{
				piece.setPieceValue(3);
				piece.setPieceType(Knight);
			}
			else if(i == 2 || i == 5) // bishop
			{
				piece.setPieceValue(3);
				piece.setPieceType(Bishop);
			}
			else if(i == 3) // queen
			{
				piece.setPieceValue(9);
				piece.setPieceType(Queen);
			}
			else if(i == 4) // king
			{
				piece.setPieceValue(10);
				piece.setPieceType(King);
			}
			else // pawn
			{
				piece.setPieceValue(1);
				piece.setPieceType(Pawn);
			}
			
			board.push_back(piece);
		}

		else if(BOARD_SIZE/4 <= i && i < BOARD_SIZE*3/4) // blank squares
		{	
			piece.setPieceSquare(i);
			piece.setPieceValue(0);
			piece.setPieceType(Empty);
			piece.setPieceColor(Neutral);
			board.push_back(piece);
		}
		else // white
		{
			piece.setPieceSquare(i);
			if(i == 56 || i == 63) // rook
			{
				piece.setPieceValue(5);
				piece.setPieceType(Rook);
			}
			else if(i == 57 || i == 62) // knight
			{
				piece.setPieceValue(3);
				piece.setPieceType(Knight);
			}
			else if(i == 58 || i == 61) // bishop
			{
				piece.setPieceValue(3);
				piece.setPieceType(Bishop);
			}
			else if(i == 59) // queen
			{
				piece.setPieceValue(9);
				piece.setPieceType(Queen);
			}
			else if(i == 60) // king
			{
				piece.setPieceValue(10);
				piece.setPieceType(King);
			}
			else // pawn
			{
				piece.setPieceValue(1);
				piece.setPieceType(Pawn);
			}
			
			piece.setPieceColor(White);
			board.push_back(piece);
		}
	}

	return board;
}

// Print the current board position
void printBoard(const vector<Switcher> & v)
{
	cout << endl;

	char temp;
	vector<Switcher>::const_iterator itr; // due to const in the signature, this must be const

	int count = 0;
	for(itr = v.begin(); itr != v.end(); itr++)
	{
		switch(itr->getPieceType())
		{
			case Rook:
				temp = itr->getPieceColor() == White ? 'R' : 'r';
				break;
			case Knight:
				temp = itr->getPieceColor() == White ? 'N' : 'n';
				break;
			case Bishop:
				temp = itr->getPieceColor() == White ? 'B' : 'b';
				break;
			case King:
				temp = itr->getPieceColor() == White ? 'K' : 'k';
				break;
			case Queen:
				temp = itr->getPieceColor() == White ? 'Q' : 'q';
				break;
			case Pawn:
				temp = itr->getPieceColor() == White ? 'P' : 'p';
				break;
			default:
				temp = '.';
		}

		cout << std::right << std::setw(2) << temp;

		// go to next row if reached last column
		if(count % 8 == 7)
			cout << endl;
		count++;
	}
}