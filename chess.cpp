#include "chess.h"

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
bool Chess::getPieceMovevement() const
{
	return moved;
}

void Chess::setPieceMovement()
{
	this->moved = !this->moved; // flip if piece was moved
}

// Checks if a given move is valid according to objects type and 'src' & 'dest' square coordinates
// Return 'true' if move is valid, 'false' otherwise
bool Chess::isValidMove(int src, int dest, const vector<Chess> & board)
{
	bool valid;
	vector<int> bit_map_row, bit_map_col, bit_map_diagl, bit_map_diagr;

	int src_row = src / 8, src_col = src % 8;
	int dest_row = dest / 8, dest_col = dest % 8;
	int diff = (dest - src) > 0 ? dest - src : src - dest;

	switch(this->getPieceType())
	{
		case Rook: // moves in same row and column
			valid = (src_row == dest_row || src_col == dest_col) && freePath(src, dest, board);
			break;

		case Knight: // knight can jump over obstacles
			valid = abs(src_row - dest_row) <= 2 && abs(src_col - dest_col) <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17) && board[dest].getPieceColor() != board[src].getPieceColor();
			break;

		case Bishop:
			valid = (diff % 7 == 0 || diff % 9 == 0) && (abs(src_row-dest_row) == abs(src_col-dest_col)) && freePath(src, dest, board);;
			break;

		case King: // like rook and bishop but with end square being 1 away
			valid = (diff == 1 || diff == 7 || diff == 8|| diff == 9) && board[dest].getPieceColor() != board[src].getPieceColor();
			break;

		case Queen: // like rook and bishop
			valid = (((diff % 7 == 0 || diff % 9 == 0) && (abs(src_row-dest_row) == abs(src_col-dest_col))) || (src_row == dest_row || src_col == dest_col)) && freePath(src, dest, board);
			break;

		case Pawn: // on attack, it can move sideways & first move can be 2 squares forward
			if(this->getPieceMovevement())
			{
				if(this->getPieceColor() == White) // goes up
					valid = (src-dest == 8 && board[dest].getPieceType() == Empty) || ((src-dest == 7 || src-dest == 9) && board[dest].getPieceType() != Empty);
				else // black, goes down
					valid = (dest-src == 8 && board[dest].getPieceType() == Empty) || ((dest-src == 7 || dest-src == 9) && board[dest].getPieceType() != Empty);
			}
			else
			{	
				if(this->getPieceColor() == White) // goes up
					valid = ((src-dest == 8 || src-dest == 16) && board[dest].getPieceType() == Empty) || ((src-dest == 7 || src-dest == 9) && board[dest].getPieceType() != Empty);
				else // black, goes down
					valid = ((dest-src == 8 || dest-src == 16) && board[dest].getPieceType() == Empty) || ((dest-src == 7 || dest-src == 9) && board[dest].getPieceType() != Empty);
			}

			valid = valid && freePath(src, dest, board);
			break;

		default:
			valid = false;
	}
    
    return valid;
}

// Determines if the path from 'src' to 'dest' contains any pieces (Return 0) or not (Return 1)
// Also makes sure that piece on 'dest' is not the same color as 'src'
bool Chess::freePath(int src, int dest, const vector<Chess> & board)
{
	bool empty = true;

	int src_row = src / 8, src_col = src % 8;
	int dest_row = dest / 8, dest_col = dest % 8;

	// make sure 'src' is always lower so can simply analyze one way (same whichever way you choose)
	if(src > dest) 
	{
		std::swap(src, dest);
	}

	if(src_row == dest_row) // row path
	{
		for(int i = src+1; i<dest; i++)
		{
			if(board[i].getPieceType() != Empty)
				empty = false;
		}
	}
	else if(src_col == dest_col) // column path
	{
		for(int i = src+8; i<dest; i+=8)
		{
			if(board[i].getPieceType() != Empty)
				empty = false;
		}
	}
	else if(abs(src_row - dest_row) == abs(src_col - dest_col)) // diagonal path
	{
		// figure out which direction the diagonal is in
		int increment;
		if(abs(dest-src) % 7 == 0)
			increment = 7;
		else
			increment = 9;

		for(int i = src+increment; i<dest; i+=increment)
		{
			if(board[i].getPieceType() != Empty)
				empty = false;
		}
	}

	return empty && board[src].getPieceColor() != board[dest].getPieceColor();
}

// if move is valid, make the move
// On return, the piece's square value is updated
void Chess::makeMove(int dest, vector<Chess> & board, int & turn, bool valid)
{
	int src = this->getPieceSquare();
	
	if(0 <= dest && dest <= 63 && (dest-src) != 0 && board[src].getPieceColor() == turn+1 && this->isValidMove(src, dest, board))
	{
		if(!checkDetect(src, dest, board))
		{
			// pawn promotion
			if(this->getPieceType() == Pawn && (dest/8 == 0 || dest/8 == 7))
			{
				this->promotePawn();
			}

			// note that the piece moved (important for castling and pawn's first move)
			this->setPieceMovement(); 
			board = moveChoice(src, dest, board);

			valid = true;
		}
	}
		
	if(valid)
	{
		printBoard(board);
		playerTurn(turn);
	}
	else
	{
		cout << endl << "Invalid move, try again!" << endl;
	}
}

// Decide if it is an attacking move or regular move
vector<Chess> Chess::moveChoice(int src, int dest, vector<Chess> board)
{
	if(board[dest].getPieceType() == Empty)
	{
		board[src].setPieceSquare(dest);
		board[dest].setPieceSquare(src);
		std::swap(board[src], board[dest]);
	}
	else
	{
		board = board[src].attackMove(src, dest, board);
	}

	return board;
}

// When a piece attacks another, cannot simply swap, must replace 'dest' piece while making 'src' blank
vector<Chess> Chess::attackMove(int src, int dest, vector<Chess> board)
{
	// dest
	board[dest].setPieceType(board[src].getPieceType());
	board[dest].setPieceValue(board[src].getPieceValue());
	board[dest].setPieceColor(board[src].getPieceColor());
	board[dest].setPieceSquare(dest);

	// src
	board[src].setPieceType(Empty);
	board[src].setPieceValue(0);
	board[src].setPieceColor(Neutral);
	board[src].setPieceSquare(src);

	return board;
}

// A valid move was made
// Return true if king is in check, otherwise return false
bool Chess::checkDetect(int src, int dest, vector<Chess> board)
{
	board = moveChoice(src, dest, board); // make the move without affecting the main board!
	if(checkStack.empty())
	{
		int king_pos;
		vector<Chess>::const_iterator itr;
		for(itr = board.begin(); itr != board.end(); itr++)
		{
			if(itr->getPieceType() == King && itr->getPieceColor() != board[dest].getPieceColor())
			{
				king_pos = itr->getPieceSquare();
				break;
			}
		}

		if(board[dest].isValidMove(dest, king_pos, board)) 
		{
			cout << "Check!";
			checkStack.push(board[king_pos]);
			checkStack.push(board[dest]);
		}

		return false;
	}

	else // checkStack is not empty (a player must be in check!)
	{
		bool result;
		Chess piece, king;

		piece = checkStack.top();
		checkStack.pop();
		king = checkStack.top();
		checkStack.pop();

		// the player's king made the move to try to get out of check
		if(board[dest].getPieceType() == King)
		{
			king.setPieceSquare(dest);
		}
		result = piece.isValidMove(piece.getPieceSquare(), king.getPieceSquare(), board);

		// push back on stack if move was not valid
		if(result)
		{
			checkStack.push(king);
			checkStack.push(piece);
		}

		return result;
	}
}

// When a pawn reaches the end of the board, it can be exchanged for either a queen, rook, bishop or knight
void Chess::promotePawn()
{
	char piece;

	while(true)
	{
		cout << "Which Piece: Q/q | R/r | B/b | N/n?";
		cin >> piece;
		if(piece == 'Q' || piece == 'q')
		{
			this->setPieceValue(9);
			this->setPieceType(Queen);
			break;
		}
		if(piece == 'R' || piece == 'r')
		{
			this->setPieceValue(5);
			this->setPieceType(Rook);
			break;
		}
		if(piece == 'B' || piece == 'b')
		{
			this->setPieceValue(3);
			this->setPieceType(Bishop);
			break;
		}
		if(piece == 'N' || piece == 'n')
		{
			this->setPieceValue(3);
			this->setPieceType(Knight);
			break;
		}

		cout << "Please make sure to pick correct value!" << endl;
	}
}

// Board intialization
void initBoard(vector<Chess> & board)
{
	// initialize the board
	for(unsigned int i = 0; i < board.size(); i++)
	{
		if(i < board.size()/4) // black
		{
			board[i].setPieceSquare(i);
			if(i == 0 || i == 7) // rook
			{
				board[i].setPieceValue(5);
				board[i].setPieceType(Rook);
			}
			else if(i == 1 || i == 6) // knight
			{
				board[i].setPieceValue(3);
				board[i].setPieceType(Knight);
			}
			else if(i == 2 || i == 5) // bishop
			{
				board[i].setPieceValue(3);
				board[i].setPieceType(Bishop);
			}
			else if(i == 3) // queen
			{
				board[i].setPieceValue(9);
				board[i].setPieceType(Queen);
			}
			else if(i == 4) // king
			{
				board[i].setPieceValue(10);
				board[i].setPieceType(King);
			}
			else // pawn
			{
				board[i].setPieceValue(1);
				board[i].setPieceType(Pawn);
			}
			
			board[i].setPieceColor(Black);
		}

		else if(board.size()/4 <= i && i < board.size()*3/4) // blank squares
		{	
			board[i].setPieceSquare(i);
		}
		else // white
		{
			board[i].setPieceSquare(i);
			if(i == 56 || i == 63) // rook
			{
				board[i].setPieceValue(5);
				board[i].setPieceType(Rook);
			}
			else if(i == 57 || i == 62) // knight
			{
				board[i].setPieceValue(3);
				board[i].setPieceType(Knight);
			}
			else if(i == 58 || i == 61) // bishop
			{
				board[i].setPieceValue(3);
				board[i].setPieceType(Bishop);
			}
			else if(i == 59) // queen
			{
				board[i].setPieceValue(9);
				board[i].setPieceType(Queen);
			}
			else if(i == 60) // king
			{
				board[i].setPieceValue(10);
				board[i].setPieceType(King);
			}
			else // pawn
			{
				board[i].setPieceValue(1);
				board[i].setPieceType(Pawn);
			}
			
			board[i].setPieceColor(White);
		}
	}

	cout << "\nWhite to move first (as always)!" << endl;
	printBoard(board);
}

// Print the current board position
void printBoard(const vector<Chess> & board)
{
	cout << endl;

	char temp;
	vector<Chess>::const_iterator itr; // due to const in the signature, this must be const

	int count = 0;
	for(itr = board.begin(); itr != board.end(); itr++)
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

		cout << std::left << std::setw(2) << temp;

		// go to next row if reached last column
		if(count % 8 == 7)
			cout << endl;
		count++;
	}
}

// Print whose turn it is after a move is played
void playerTurn(int & turn)
{
	turn = turn == 1 ? -1 : 1; // switch turns
	if(turn == 1)
	{
		cout << endl << "White's turn" << endl;
	}
	else
	{
		cout << endl << "Black's turn" << endl;
	}
}