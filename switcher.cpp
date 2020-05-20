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


// Checks if a given move is valid according to objects type and 'start' & 'end' square coordinates
// Return 'true' if move is valid, 'false' otherwise
bool Switcher::isValidMove(unsigned int start_square, unsigned int end_square) const
{
	/* TODO: 
		1. add attacking move for pawn
		2. add piece barrier conditions
		3. Knight conditions must be stricter
	*/

	bool valid;
	unsigned int diff = end_square - start_square > 0 ? end_square - start_square : start_square - end_square;

	switch(this->getPieceType())
	{
		case Rook:
			valid = (diff % 8 == 0 && diff != 0);
			break;
		case Knight:
			valid = (diff == 6 || diff == 10 || diff == 15 || diff == 17);
			break;
		case Bishop:
			valid = (diff % 7 == 0 && diff != 0);
			break;
		case King:
			valid = (diff == 1 && diff != 0);
			break;
		case Queen:
			valid = (diff % 7 == 0 && diff != 0);
			break;
		case Pawn: // need to add attacks
			if(this->getPieceMovevement())
				valid = (diff == 1 && diff != 0);
			else
			{
				valid = ((diff == 1 || diff == 2) && diff != 0);
			}
			break;
		default:
			valid = false;
	}
    
    return valid;
}

// if move is valid, make the move
// On return, the piece's square value is updated
bool Switcher::makeMove(vector<Switcher> & board, unsigned int dest)
{
	if(this->isValidMove(this->getPieceSquare(), dest))
	{
		std::swap(*this, board[dest]);
		return true;
	}
	
	cout << endl << "Invalid move, try again!" << endl;
	return false;
}


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