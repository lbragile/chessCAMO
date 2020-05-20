#include "switcher.h"
#include <vector>

using namespace std;

#define BOARD_SIZE 64

int main()
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

	printBoard(board);
	return 0;
}