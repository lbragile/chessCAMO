#include "chess.h"

Chess chess; // global object call

Piece::~Piece() {}
Pawn::~Pawn() {}
Knight::~Knight() {}
Bishop::~Bishop() {}
Rook::~Rook() {}
Queen::~Queen() {}
King::~King() {}
Empty::~Empty() {}

// // copy assignment
// Chess & Chess::operator =(const Chess & object){}
// // copy assignment
// Piece & Piece::operator =(const Piece & object){}
// // copy assignment
// Pawn & Pawn::operator =(const Pawn & object){}
// // copy assignment
// Knight & Knight::operator =(const Knight & object){}
// // copy assignment
// Bishop & Bishop::operator =(const Bishop & object){}
// // copy assignment
// Rook & Rook::operator =(const Rook & object){}
// // copy assignment
// Queen & Queen::operator =(const Queen & object){}
// // copy assignment
// King & King::operator =(const King & object){}
// // copy assignment
// Empty & Empty::operator =(const Empty & object){}

Chess::~Chess()
{
	while(!board.empty())
	{
		board.pop_back(); // destroy the piece
	}
}

void Chess::makeMove(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	PlayerTurn turn = chess.getTurn();
    if(0 <= dest && dest <= 63 && dest != src && turn == WhiteTurn && board[src]->isLegalMove(dest))
    {
        // // pawn promotion
        // if(this->isPawn() && (dest/8 == 0 || dest/8 == 7))
        //     this->promotePawn();

        makeMoveForType(src, dest);
        chess.setTurn(turn == 2 ? BlackTurn : WhiteTurn);
  		printBoard(chess.getBoard());
    }
}

void Chess::makeMoveForType(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	// if(isCastled(src, dest, board))
	// {
	//     // note that the pieces are moved
	//     board[src]->setPieceMovement();
	//     board[dest]->setPieceMovement();

	//     if(abs(src - dest) == 3) // king side castle
	//     {
 //            // king move
 //            board[src]->setPieceSquare(src + 2);
 //            board[src + 2]->setPieceSquare(src);
 //            std::swap(board[src], board[src + 2]);

 //            // rook move
 //            board[dest]->setPieceSquare(dest - 2);
 //            board[dest - 2]->setPieceSquare(dest);
 //            std::swap(board[dest - 2], board[dest]);
	//     }

	//     else // abs(src-dest) == 4 -> queen side castle
	//     {
 //            // king move
 //            board[src]->setPieceSquare(src - 2);
 //            board[src - 2]->setPieceSquare(src);
 //            std::swap(board[src], board[src - 2]);

 //            // rook move
 //            board[dest]->setPieceSquare(dest + 3);
 //            board[dest + 3]->setPieceSquare(dest);
 //            std::swap(board[dest + 3], board[dest]);
	//     }
	// }

	// else
	// {
	    // note that the piece moved
	    board[src]->setPieceMoveInfo();

	    // Regular Move
	    if(board[dest]->isEmpty())
	    {
            board[src]->setPieceSquare(dest);
            board[dest]->setPieceSquare(src);
            std::swap(board[src], board[dest]);
	    }

	    // Attacking Move
	    else
	    {
            // dest
            board[dest]->setPieceType(board[src]->getPieceType());
            board[dest]->setPieceValue(board[src]->getPieceValue());
            board[dest]->setPieceColor(board[src]->getPieceColor());
            board[dest]->setPieceSquare(dest);

            // src
            board[src]->setPieceType(EMPTY);
            board[src]->setPieceValue(0);
            board[src]->setPieceColor(Neutral);
            board[src]->setPieceSquare(src);
	    }
	// }
	   chess.setBoard(board);
}

bool Piece::isPathFree(int src, int dest)
{
    int src_row = src / 8, src_col = src % 8;
    int dest_row = dest / 8, dest_col = dest % 8;

    // make sure 'src' is always lower so can simply analyze one way (same whichever way you choose)
    if(src > dest)
    {
        std::swap(src, dest);
    }

    if(src_row == dest_row) // row path
    {
        return pathIterator(src, dest, 1) && !isSameColor(src, dest);
    }
    else if(src_col == dest_col) // column path
    {
        return pathIterator(src, dest, 8) && !isSameColor(src, dest);
    }
    else if(abs(src_row - dest_row) == abs(src_col - dest_col)) // diagonal path
    {
        return pathIterator(src, dest, abs(src - dest) % 7 == 0 ? 7 : 9) && !isSameColor(src, dest);
    }
    else
    {
    	return false;
    }
}

bool Piece::pathIterator(int src, int dest, int increment)
{
	vector<Piece*> board = chess.getBoard();
	for(int i = src+increment; i<dest; i+=increment)
    {
        if(!board[i]->isEmpty())
            return false;
    }
    return true;
}

bool Piece::isSameColor(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	return board[src]->getPieceColor() == board[dest]->getPieceColor();
}

bool Piece::isLegalMove(int dest)
{
	if(this->isEmpty())
	{
		return false;
	}	
	else
	{
		return this->isLegalMove(dest);
	}	
}

bool Pawn::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	bool legal = false;
	int src = this->getPieceSquare();

	// on attack, it can move sideways & first move can be 2 squares forward
    if(this->getPieceMoveInfo())
    {
        if(this->isPieceWhite()) // goes up
            legal = (src-dest == 8 && board[dest]->isEmpty()) || ((src-dest == 7 || src-dest == 9) && !board[dest]->isEmpty());
        else // black, goes down
            legal = (dest-src == 8 && board[dest]->isEmpty()) || ((dest-src == 7 || dest-src == 9) && !board[dest]->isEmpty());
    }
    else
    {
        if(this->isPieceWhite()) // goes up
            legal = ((src-dest == 8 || src-dest == 16) && board[dest]->isEmpty()) || ((src-dest == 7 || src-dest == 9) && !board[dest]->isEmpty());
        else // black, goes down
            legal = ((dest-src == 8 || dest-src == 16) && board[dest]->isEmpty()) || ((dest-src == 7 || dest-src == 9) && !board[dest]->isEmpty());
    }

    return legal && isPathFree(src, dest);
}

bool Rook::isLegalMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	return (src_row == dest_row || src_col == dest_col) && isPathFree(src, dest);
}

bool Knight::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return diff_row <= 2 && diff_col <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17) && !isSameColor(src, dest);
}

bool Bishop::isLegalMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return (diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col && isPathFree(src, dest);
}

bool Queen::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return (((diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col) || (src_row == dest_row || src_col == dest_col)) && isPathFree(src, dest);
}

bool King::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest);
	return (src_row == dest_row || src_col == dest_col || diff % 7 == 0 || diff % 9 == 0) && diff == 1;
}

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
				board.push_back(new Rook(i, 5, ROOK, Black));
			}
			else if(i == 1 || i == 6) // knight
			{
				board.push_back(new Knight(i, 3, KNIGHT, Black)); 
			}
			else if(i == 2 || i == 5) // bishop
			{
				board.push_back(new Bishop(i, 3, BISHOP, Black)); 
			}
			else if(i == 3) // queen
			{
				board.push_back(new Queen(i, 9, QUEEN, Black));
			}
			else if(i == 4) // king
			{
				board.push_back(new King(i, 10, KING, Black));
			}
			else // pawn
			{
				board.push_back(new Pawn(i, 1, PAWN, Black));
			}
		}
		else if(board_size/4 <= i && i < board_size*3/4) // blank squares
		{	
			board.push_back(new Empty(i, 0, EMPTY, Neutral));
		}
		else // white
		{
			if(i == 56 || i == 63) // rook
			{
				board.push_back(new Rook(i, 5, ROOK, White));
			}
			else if(i == 57 || i == 62) // knight
			{
				board.push_back(new Knight(i, 3, KNIGHT, White)); 
			}
			else if(i == 58 || i == 61) // bishop
			{
				board.push_back(new Bishop(i, 3, BISHOP, White));
			}
			else if(i == 59) // queen
			{
				board.push_back(new Queen(i, 9, QUEEN, White));
			}
			else if(i == 60) // king
			{
				board.push_back(new King(i, 10, KING, White));
			}
			else // pawn
			{
				board.push_back(new Pawn(i, 1, PAWN, White));
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
