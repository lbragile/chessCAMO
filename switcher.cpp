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

// // Checks if a given move is valid according to objects type and 'start' & 'end' square coordinates
// // Return 'true' if move is valid, 'false' otherwise
// bool Switcher::isValidMove(unsigned int start_square, unsigned int end_square) const
// {

// }

// Print the current board position
void printBoard(const vector<Switcher> & v)
{
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

		cout << temp << std::setw(2);

		// go to next row if reached last column
		if(count % 8 == 7)
			cout << endl;
		count++;

	}
}