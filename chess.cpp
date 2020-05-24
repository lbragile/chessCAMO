#include "chess.h"

// default constructor
Chess::Chess() : checkmate{false}, stalemate{false}, check{false}, turn{WhiteTurn}
{
 	board.resize(64);
}

// constructor with piece initialization
Piece::Piece(int value, pieceType type, pieceColor color) : moved{false}, pinned{false}
{
	this->value = value;
	this->type = type;
	this->color = color;
}

// Board intialization
void boardInit(Chess & chess)
{
	vector<Piece> board = chess.getBoard();

	Piece bRook(5, Rook, Black), bKnight(3, Knight, Black), bBishop(3, Bishop, Black),
		  bQueen(9, Queen, Black), bKing(10, King, Black), bPawn(1, Pawn, Black);
	Piece wRook(5, Rook, White), wKnight(3, Knight, White), wBishop(3, Bishop, White),
		  wQueen(9, Queen, White), wKing(10, King, White), wPawn(1, Pawn, White);
	Piece Blank;

	// initialize the board
	for(unsigned int i = 0; i < board.size(); i++)
	{
		if(i < board.size()/4) // black
		{
			if(i == 0 || i == 7) // rook
			{
				board[i] = bRook.setPieceSquare(i);
			}
			else if(i == 1 || i == 6) // knight
			{
				board[i] = bKnight.setPieceSquare(i); 
			}
			else if(i == 2 || i == 5) // bishop
			{
				board[i] = bBishop.setPieceSquare(i);
			}
			else if(i == 3) // queen
			{
				board[i] = bQueen.setPieceSquare(i);
			}
			else if(i == 4) // king
			{
				board[i] = bKing.setPieceSquare(i);
			}
			else // pawn
			{
				board[i] = bPawn.setPieceSquare(i);
			}
		}

		else if(board.size()/4 <= i && i < board.size()*3/4) // blank squares
		{	
			board[i] = Blank.setPieceSquare(i);
		}
		else // white
		{
			if(i == 56 || i == 63) // rook
			{
				board[i] = wRook.setPieceSquare(i);
			}
			else if(i == 57 || i == 62) // knight
			{
				board[i] = wKnight.setPieceSquare(i);
			}
			else if(i == 58 || i == 61) // bishop
			{
				board[i] = wBishop.setPieceSquare(i);
			}
			else if(i == 59) // queen
			{
				board[i] = wQueen.setPieceSquare(i);
			}
			else if(i == 60) // king
			{
				board[i] = wKing.setPieceSquare(i);
			}
			else // pawn
			{
				board[i] = wPawn.setPieceSquare(i);
			}
		}
	}

	chess.setBoard(board);
	
	cout << "\nWhite to move first (as always)! \n" << endl;
	printBoard(board);
}

// Print the current board position
void printBoard(const vector<Piece> & board)
{
	char piece_char;
	
	int count = 0;
	for(auto elem : board)
	{
		switch(elem.getPieceType())
		{
			case Rook:
				piece_char = elem.getPieceColor() == White ? 'R' : 'r';
				break;
			case Knight:
				piece_char = elem.getPieceColor() == White ? 'N' : 'n';
				break;
			case Bishop:
				piece_char = elem.getPieceColor() == White ? 'B' : 'b';
				break;
			case King:
				piece_char = elem.getPieceColor() == White ? 'K' : 'k';
				break;
			case Queen:
				piece_char = elem.getPieceColor() == White ? 'Q' : 'q';
				break;
			case Pawn:
				piece_char = elem.getPieceColor() == White ? 'P' : 'p';
				break;
			default:
				piece_char = '.';
		}

		cout << std::left << std::setw(2) << piece_char;

		// go to next row if reached last column
		if(count % 8 == 7)
			cout << endl;
		count++;
	}
}
