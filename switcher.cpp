#include "switcher.h"
#include <iomanip>
#include <vector>
#include <stdlib.h>     /* abs */

using namespace std;

// // copy assignment
// Chess & Chess::operator =(const Chess & object)
// {
// 	this->square = object.square;
// 	this->value = object.value;
// 	this->type = object.type;
// 	this->color = object.color;
// 	this->moved = object.moved;

// 	return *this;
// }

// // move assignment
// Chess  &Chess::operator =(Chess && object)
// {
// 	*this = std::move(object);
// 	return *this;
// }

// default constructor
Chess::Chess() : square{0}, value{0}, type{Empty}, color{Neutral}, moved{false}
{
 // intentionally blank
}

// constructor with piece initialization
Chess::Chess(unsigned int square, unsigned int value, pieceType type, pieceColor color) : moved{false}
{
	this->square = square;
	this->value = value;
	this->type = type;
	this->color = color;
}

// Mutator and accessor functions for determining/setting the piece type of an object
pieceType Chess::getPieceType() const
{
	return type;
}

void Chess::setPieceType(pieceType type)
{
	this->type = type;
}

// Mutator and accessor functions for determining/setting the piece color of an object
pieceColor Chess::getPieceColor() const
{
	return color;
}

void Chess::setPieceColor(pieceColor color)
{
	this->color = color;
}

// Mutator and accessor functions for determining/setting the piece value of an object
unsigned int Chess::getPieceValue() const
{
	return value;
}

void Chess::setPieceValue(unsigned int value)
{
	this->value = value;
}

// Mutator and accessor functions for determining/setting the piece square of an object
unsigned int Chess::getPieceSquare() const
{
	return square;
}

void Chess::setPieceSquare(unsigned int square)
{
	this->square = square;
}

// Mutator and accessor functions for determining/setting the moving state of an object
unsigned int Chess::getPieceMovevement() const
{
	return moved;
}

void Chess::setPieceMovement(bool movement)
{
	moved = movement;
}


// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
// Return 'true' if move is valid, 'false' otherwise
bool Chess::isValidMove(int src, int dest, vector<Chess> & board) const
{
	/* TODO: 
		1. add attacking move for pawn
		2. add piece barrier conditions
		3. castling
		4. move into square with a piece of opponent (attack)
		5. Promotion?
	*/
	bool valid;

	int src_row = src / 8, src_col = src % 8;
	int dest_row = dest / 8, dest_col = dest % 8;
	int diff = (dest - src) > 0 ? dest - src : src - dest;

	switch(this->getPieceType())
	{
		case Rook: // moves in same row and column
			valid = (src_row == dest_row || src_col == dest_col);
			break;

		case Knight: 
			valid = abs(src_row - dest_row) <= 2 && abs(src_col - dest_col) <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17);
			break;

		case Bishop:
			valid = (diff % 7 == 0 || diff % 9 == 0) && (abs(src_row-dest_row) == abs(src_col-dest_col));
			break;

		case King: // like rook and bishop but with end square being 1 away
			valid = (src_row == dest_row || src_col == dest_col || diff % 7 == 0 || diff % 9 == 0) && diff == 1;
			break;

		case Queen: // like rook and bishop
			valid = (((diff % 7 == 0 || diff % 9 == 0) && (abs(src_row-dest_row) == abs(src_col-dest_col))) || (src_row == dest_row || src_col == dest_col));
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

			break;

		default:
			valid = false;
	}
    
    return valid;
}

// if move is valid, make the move
// On return, the piece's square value is updated
bool Chess::makeMove(vector<Chess> & board, int dest)
{
	/* TODO:
		For pawns this condition must be stricter
		Likewise for castling
	*/
	int src = this->getPieceSquare();
	if(this->isValidMove(src, dest, board))
	{
		this->setPieceMovement(true); // note that the piece moved (important for castling and pawn's first move)
		this->setPieceSquare(dest);
		board[dest].setPieceSquare(src);
		std::swap(*this, board[dest]);
		return true;
	}
	
	return false;
}

// Converts a row, column, or diagonal of the current board state into a bit vector
// Returns a bit vector where -1 (black), 0 (blank), 1 (white)
vector<int> Chess::bitify(pieceDirection dir, const vector<Chess> & board)
{
	int src = this->getPieceSquare();
	int row, col, diag_l, diag_r;
	row = src - (src%8);
	col = src - row;
	diag_l = src%8 < src/8 ? src - 7*(src%8) : src - 7*(src/8);
	diag_r = src%8 < src/8 ? src - 9*(src%8) : src - 9*(src/8);

	cout << row << " " << col << " " << diag_l << " " << diag_r << endl;

	vector<int> bit_map;
	for(int i = 0; i < 8; i++)
	{
		if(dir == Row)
		{
			switch(board[row].getPieceColor())
			{
				case 0:
					bit_map.push_back(-1);
					break;
				case 1:
					bit_map.push_back(0);
					break;
				case 2:
					bit_map.push_back(1);
					break;
			}

			row++;
		}
		else if(dir == Col)
		{
			switch(board[col].getPieceColor())
			{
				case 0:
					bit_map.push_back(-1);
					break;
				case 1:
					bit_map.push_back(0);
					break;
				case 2:
					bit_map.push_back(1);
					break;
			}

			col += 8;
		}
		else if(dir == Diag_L) // doesn't have to be size 8, e.g. if on the first column already
		{
			switch(board[diag_l].getPieceColor())
			{
				case 0:
					bit_map.push_back(-1);
					break;
				case 1:
					bit_map.push_back(0);
					break;
				case 2:
					bit_map.push_back(1);
					break;
			}

			if(diag_l % 8 == 0)
			{
				break;
			}

			diag_l += 7;
		}
		else // dir == Diag_R
		{
			switch(board[diag_r].getPieceColor())
			{
				case 0:
					bit_map.push_back(-1);
					break;
				case 1:
					bit_map.push_back(0);
					break;
				case 2:
					bit_map.push_back(1);
					break;
			}

			if(diag_r % 8 == 7)
			{
				break;
			}

			diag_r += 9;
		}
	}
	return bit_map;	
}

// Board intialization
vector<Chess> initBoard(unsigned int BOARD_SIZE)
{
	// 8x8 board stored as vector of vectors, each row contains the piece objects
	vector<Chess> board; 

	// initialize the board
	for(unsigned int i = 0; i < BOARD_SIZE; i++)
	{
		Chess piece;

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
			
			piece.setPieceColor(Black);
			board.push_back(piece);
		}

		else if(BOARD_SIZE/4 <= i && i < BOARD_SIZE*3/4) // blank squares
		{	
			piece.setPieceSquare(i);
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
void printBoard(const vector<Chess> & v)
{
	cout << endl;

	char temp;
	vector<Chess>::const_iterator itr; // due to const in the signature, this must be const

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