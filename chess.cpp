#include "chess.h"

// bool Piece::isValidMove(int dest, vector<Piece> board)
// {
// 	if(this->isPawn())
// 		return this->Pawn::isLegalMove(dest, board);
// 	// else if(this->isKnight())
// 	// 	return this->Knight::isLegalMove(dest, board);
// 	// else if(this->isBishop())
// 	// 	return this->Bishop::isLegalMove(dest, board);
// 	// else if(this->isRook())
// 	// 	return this->Rook::isLegalMove(dest, board);
// 	// else if(this->isQueen())
// 	// 	return this->Queen::isLegalMove(dest, board);
// 	// else if(this->isKing())
// 	// 	return this->King::isLegalMove(dest, board);
// 	else // empty
// 		return false;
// }

// bool Pawn::isLegalMove(int dest, vector<Piece> board)
// {
// 	bool legal = false;
// 	int src = this->getPieceSquare();

// 	// on attack, it can move sideways & first move can be 2 squares forward
//     if(this->getPieceMoveInfo())
//     {
//         if(this->isPieceWhite()) // goes up
//             legal = (src-dest == 8 && board[dest].isEmpty()) || ((src-dest == 7 || src-dest == 9) && !board[dest].isEmpty());
//         else // black, goes down
//             legal = (dest-src == 8 && board[dest].isEmpty()) || ((dest-src == 7 || dest-src == 9) && !board[dest].isEmpty());
//     }
//     else
//     {
//         if(this->isPieceWhite()) // goes up
//             legal = ((src-dest == 8 || src-dest == 16) && board[dest].isEmpty()) || ((src-dest == 7 || src-dest == 9) && !board[dest].isEmpty());
//         else // black, goes down
//             legal = ((dest-src == 8 || dest-src == 16) && board[dest].isEmpty()) || ((dest-src == 7 || dest-src == 9) && !board[dest].isEmpty());
//     }

//     return legal;
//     // return legal && freePath(src, dest, board);
// }

// Board intialization
void boardInit(Chess & chess)
{
	int board_size = 64;
	vector<Piece*> board;

	// initialize the board
	for(int i = 0; i < board_size; i++)
	{
		if(i < board_size/4) // black
		{
			if(i == 0 || i == 7) // rook
			{
				board.push_back(new Rook(5, ROOK, Black));
			}
			else if(i == 1 || i == 6) // knight
			{
				board.push_back(new Knight(3, KNIGHT, Black)); 
			}
			else if(i == 2 || i == 5) // bishop
			{
				board.push_back(new Bishop(3, BISHOP, Black)); 
			}
			else if(i == 3) // queen
			{
				board.push_back(new Queen(9, QUEEN, Black));
			}
			else if(i == 4) // king
			{
				board.push_back(new King(10, KING, Black));
			}
			else // pawn
			{
				board.push_back(new Pawn(1, PAWN, Black));
			}
		}
		else if(board_size/4 <= i && i < board_size*3/4) // blank squares
		{	
			board.push_back(new Empty(0, EMPTY, Neutral));
		}
		else // white
		{
			if(i == 56 || i == 63) // rook
			{
				board.push_back(new Rook(5, ROOK, White));
			}
			else if(i == 57 || i == 62) // knight
			{
				board.push_back(new Knight(3, KNIGHT, White)); 
			}
			else if(i == 58 || i == 61) // bishop
			{
				board.push_back(new Bishop(3, BISHOP, White));
			}
			else if(i == 59) // queen
			{
				board.push_back(new Queen(9, QUEEN, White));
			}
			else if(i == 60) // king
			{
				board.push_back(new King(10, KING, White));
			}
			else // pawn
			{
				board.push_back(new Pawn(1, PAWN, White));
			}
		}
	}

	chess.setBoard(board);

	cout << "\nWhite to move first (as always)! \n" << endl;
	printBoard(board);
}

// Print the current board position
void printBoard(const vector<Piece*> & board)
{
	char piece_char;
	
	int count = 0;
	for(auto elem : board)
	{
		switch(elem->getPieceType())
		{
			case ROOK:
				piece_char = elem->isPieceWhite() ? 'R' : 'r';
				break;
			case KNIGHT:
				piece_char = elem->isPieceWhite() ? 'N' : 'n';
				break;
			case BISHOP:
				piece_char = elem->isPieceWhite() ? 'B' : 'b';
				break;
			case KING:
				piece_char = elem->isPieceWhite() ? 'K' : 'k';
				break;
			case QUEEN:
				piece_char = elem->isPieceWhite() ? 'Q' : 'q';
				break;
			case PAWN:
				piece_char = elem->isPieceWhite() ? 'P' : 'p';
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
