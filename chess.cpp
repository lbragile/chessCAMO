#include "chess.h"

/*****
	CHESS CLASS - MEMBER FUNCTIONS
 *****/
Chess::~Chess()
{
	vector<Piece*> board = getBoard();
	while(!board.empty())
	{
		board.pop_back();
	}
}

bool Chess::isStalemate()
{
	vector<Piece*> board = chess.getBoard();
	int possible_moves[22] = {1, 6, 7, 8, 9, 10, 14, 15, 16, 17, 18, 21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63};
	int counter = 0, move_up, move_down;

	pieceColor current_turn = chess.getTurn() == WHITE ? BLACK : WHITE;

	for(auto elem : board)
	{
		if(elem->getPieceColor() == current_turn)
		{
			for(int move : possible_moves)
			{
				move_up = elem->getPieceSquare() + move;
				move_down = elem->getPieceSquare() - move;
				if(!elem->isEmpty() && ((move_up <= 63 && elem->isLegalMove(move_up) && !elem->isPinned(move_up)) || (move_down >= 0 && elem->isLegalMove(move_down) && !elem->isPinned(move_down))))
				{
					counter++;
					break;
				}
			}
		}

		if(counter > 0)
			break;
	}

	return counter == 0;
}

void Chess::isCheckmate(string checkType)
{
	stack<Piece*> CheckStack = chess.getCheckStack();
	Piece *king, *piece;
	pieceColor current_turn = chess.getTurn() == 2 ? BLACK : WHITE;

	// will not be set, so next time this will be identical
	piece = CheckStack.top();
	CheckStack.pop();
	king = CheckStack.top();

	if(checkType == "double") // this type of check requires the king to move
	{
		// checkmate -> checkStack is not empty (a player must be in check!) AND player turn is the player not in check (has pieces not king)
		// checkmate due to a double check
		if(pieceIterator(king->getPieceSquare()) || king->getPieceColor() != current_turn)
		{
			handleCheckmate();
		}
		else
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
    		cout << "\n            Double Check!\n" << endl; // was not checkmate so can state that it is check
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
		}
	}	

	else if(checkType == "single") // CheckStack.top()->getPieceColor() != current_turn
	{
		
		// checkmate due to a single piece
		if(pieceIterator(piece->getPieceSquare(), king->getPieceSquare()) || piece->getPieceColor() == current_turn)
		{
			handleCheckmate();
		}
		else
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
    		cout << "\n                Check!\n" << endl; // was not checkmate so can state that it is check
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
		}
	}
}

bool Chess::pieceIterator(int dest)
{
	int king_moves[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

	// can king move out of check?
    for(auto move : king_moves)
    {
    	if(dest+move >= 0 && dest + move <=63 && board[dest]->isLegalMove(dest + move) && !board[dest]->movedIntoCheck(dest + move))
    	{
    		return false;
    	}
    }
    	
    return true; // no legal moves found ? true : false

}

void Chess::makeMove(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	stack<Piece*> CheckStack = chess.getCheckStack();
	pieceColor current_turn = chess.getTurn();
	Piece *king, *piece, *before_undo_piece; // "before_undo_piece" needed for attacking moves that are illegal (to restore the piece that goes missing)
	bool before_undo_moveInfo;
 	
    if(0 <= src && src <= 63 && 0 <= dest && dest <= 63 && dest != src &&
       board[src]->isLegalMove(dest) && board[src]->getPieceColor() == current_turn && !board[src]->isPinned(dest))
    {	
        // pawn promotion
        if(board[src]->isPawn() && (dest/8 == 0 || dest/8 == 7))
        {
            board[src]->promotePawn(dest);
        }

		before_undo_piece = board[dest];
		before_undo_moveInfo = board[src]->getPieceMoveInfo();

        makeMoveForType(src, dest);

        // when in double check you must move the king
	    if(chess.getDoubleCheck())
	    {      
    		king = CheckStack.top();
			CheckStack.pop();

			for(auto elem : chess.getBoard())
			{					
				if(!elem->isEmpty() && isPathFree(elem->getPieceSquare(), king->getPieceSquare()) && elem->isLegalMove(king->getPieceSquare()))
				{
					makeMoveForType(dest, src); // undo the move
					board = chess.getBoard();
					board[dest] = before_undo_piece;
					board[src]->setPieceMoveInfo(before_undo_moveInfo);
					chess.setBoard(board);

					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
					cout << "You are in double check! Try again..." << endl;
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
					return;
				}
			}

			chess.setCheckStack(CheckStack);
			chess.setDoubleCheck(false);
	    }

	    else if(chess.getCheck())
        {
        	piece = CheckStack.top();
			CheckStack.pop();
			king = CheckStack.top();
			CheckStack.pop();

			if( piece->isLegalMove(king->getPieceSquare()) && piece->isPathFree(piece->getPieceSquare(), king->getPieceSquare()) )
			{
				makeMoveForType(dest, src); // undo the move
				board = chess.getBoard();
				board[dest] = before_undo_piece;
				board[src]->setPieceMoveInfo(before_undo_moveInfo);
				chess.setBoard(board);

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
				cout << "You are in check! Try again..." << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);

				return;
			}
			else
			{
				chess.setCheckStack(CheckStack);
				chess.setCheck(false);
			}
        }

        board = chess.getBoard();
        board[dest]->enPassantHandling(src, dest);

        // did the move cause a check?
    	if(board[src]->causeCheck(dest) && !board[src]->causeDoubleCheck(dest)) 
    	{
    		chess.isCheckmate("single"); // check for checkmates
    	}

    	// did the move cause a double check?
    	if(!board[src]->causeCheck(dest) && board[src]->causeDoubleCheck(dest)) 
    	{
    		chess.isCheckmate("double"); // check for checkmates
    	}

    	// check for stalemate
    	if(chess.isStalemate()) 
		{
			chess.handleStalemate();
		}

		chess.setTurn(current_turn == 2 ? BLACK : WHITE);
		printBoard(chess.getBoard());
		if(!chess.getCheckmate())
		{
			cout << "___________________________________________________" << endl;
			if(chess.getTurn() == 2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
				cout << "\n            White's move" << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
			}
			else
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
				cout << "\n            Black's move" << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
			}
		}
  	}
    else
    {
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
		cout << "Invalid move! Try again..." << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
		
	}
}

bool Chess::isPathFree(int src, int dest)
{
	int increment;
	vector<Piece*> board = chess.getBoard();

	if(board[src]->isKnight())
	{
		return true;
	}
	else
	{
		increment = incrementChoice(src, dest);
    	return increment > 0 ? pathIterator(src, dest, increment) : false;
	}
    
}

/*****
	CHESS CLASS - HELPER FUNCTIONS
 *****/

void Chess::makeMoveForType(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();

	// castling move
	if(board[src]->canCastle(dest) && (abs(src - dest) == 3 || abs(src - dest) == 4))
	{
	    // note that the pieces are moved
	    board[src]->setPieceMoveInfo(true);
	    board[dest]->setPieceMoveInfo(true);

	    if(abs(src - dest) == 3) // king side castle
	    {
            // king move
            board[src]->setPieceSquare(src + 2);
            board[src + 2]->setPieceSquare(src);
            std::swap(board[src], board[src + 2]);

            // rook move
            board[dest]->setPieceSquare(dest - 2);
            board[dest - 2]->setPieceSquare(dest);
            std::swap(board[dest - 2], board[dest]);
	    }

	    else // abs(src-dest) == 4 -> queen side castle
	    {
            // king move
            board[src]->setPieceSquare(src - 2);
            board[src - 2]->setPieceSquare(src);
            std::swap(board[src], board[src - 2]);

            // rook move
            board[dest]->setPieceSquare(dest + 3);
            board[dest + 3]->setPieceSquare(dest);
            std::swap(board[dest + 3], board[dest]);
	    }
	}

	// en-passant move
	else if(board[src]->isPawn() && board[src]->getEnPassant() && src % 8 != dest % 8)
	{
		board[src]->setPieceSquare(dest);
        board[dest]->setPieceSquare(src);
        std::swap(board[src], board[dest]);

        // delete the pawn that caused en-passant (make it empty square)
		if(abs(src-dest) == 7)
		{
			board[src+1] = new Empty(src+1, 0, EMPTY, NEUTRAL);
		}
		else if(abs(src-dest) == 9)
		{
			board[src-1] = new Empty(src-1, 0, EMPTY, NEUTRAL);
		}
	}

	// regular or attacking
	else
	{
	    // note that the piece moved
	    board[src]->setPieceMoveInfo(true);

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
            board[dest] = board[src];
            board[dest]->setPieceSquare(dest);

            // src
            board[src] = new Empty(src, 0, EMPTY, NEUTRAL);
            board[src]->setPieceSquare(src);
	    }
	}

	chess.setBoard(board);
}

void Chess::handleCheckmate()
{
	if(chess.getTurn() == 2)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
		cout << "\n      White won by Checkmate!\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
		cout << "\n      Black won by Checkmate!\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
	}
	setCheckmate(true);
}

void Chess::handleStalemate()
{
	if(chess.getTurn() != WHITE)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
		cout << "\nWhite has no moves -> Game is Drawn!\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
		cout << "\nBlack has no moves -> Game is Drawn!\n" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
	}

	setStalemate(true);
}

bool Chess::pieceIterator(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
    int increment, original_dest = dest; // original_dest is the king
    int king_moves[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

    // can a piece defend the king from check?
    if(!board[src]->isKnight())
    {
    	increment = incrementChoice(src, dest); // passed by reference
	    for(auto elem : chess.getBoard())
	    {
	        if(elem->isSameColor(elem->getPieceSquare(), original_dest) && elem->getPieceSquare() != original_dest)
	        {
	            for(int i = src+increment; i<=dest; i+=increment)
	            {	
	                if(elem->isLegalMove(i))
	                {
	                	return false;
	                }
	            }
	        }
	    }
    }
    else // for a Knight check, to defend the king a piece must take the knight
    {
    	for(auto elem : chess.getBoard())
	    {
	        if(elem->isSameColor(elem->getPieceSquare(), dest) && elem->getPieceSquare() != dest && elem->isLegalMove(src))
	        {
	            return false;
	        }
	    }
    }

    // can king move out of check?
    for(auto move : king_moves)
    {
    	if(original_dest+move >= 0 && original_dest + move <=63 && board[original_dest]->isLegalMove(original_dest + move) && !board[original_dest]->movedIntoCheck(original_dest + move))
    	{
    		return false;
    	}
    }
    	
    return true; // no legal moves found ? true : false
}

int Chess::squareOfPieceInPath(int src, int dest)
{
	int increment;
	vector<Piece*> board = chess.getBoard();
	vector<int> pieces_in_path;

	increment = incrementChoice(src, dest);
	if(increment > 0)
	{
		for(int i = src+increment; i < dest; i += increment)
		{
			if(!board[i]->isEmpty())
			{
				pieces_in_path.push_back(i);
			}
		}
	}

	return pieces_in_path.size() == 1 ? pieces_in_path[0] : -1;
}


int Chess::incrementChoice(int & src, int & dest)
{
    // make sure 'src' is always lower so can simply analyze one way (same whichever way you choose)
	if(src > dest)
    {
    	std::swap(src, dest);
    }

    if(src/8 == dest/8) // row path
    {
        return 1;
    }
    else if(src%8 == dest%8) // column path
    {
        return 8;
    }
    else if(abs(src/8 - dest/8) == abs(src%8 - dest%8)) // diagonal path
    {
        return abs(src - dest) % 7 == 0 ? 7 : 9;
    }
    else
    {
    	return 0;
    }
}

bool Chess::pathIterator(int src, int dest, int increment)
{
	vector<Piece*> board = chess.getBoard();

	for(int i = src+increment; i<dest; i+=increment)
    {
        if(!board[i]->isEmpty())
            return false;
    }
    return true;
}

bool Chess::destInPath(int src, int dest, int pinning_piece_square)
{
	int original_src = src;
	int increment_dest = incrementChoice(src, dest); // passed by reference
	int increment_pin = incrementChoice(original_src, pinning_piece_square);

	return !(increment_dest != 0 && increment_pin != 0 && increment_dest == increment_pin && src + increment_pin <= dest);
}

/*****
	PIECE CLASS - MEMBER FUNCTIONS
 *****/
bool Piece::causeDoubleCheck(int dest)
{
	int king_pos, checking_piece_counter = 0;
	vector<Piece*> board = chess.getBoard();
	stack<Piece*> CheckStack = chess.getCheckStack();

	// causes a check if not a king (cannot check opponent with your king)
	if(this->isKing())
	{
		return false;
	}
	else
	{
		// find the king
	    for(auto elem : board)
	    {
	    	if(elem->isKing() && !isSameColor(elem->getPieceSquare(), dest))
	    	{
	    		king_pos = elem->getPieceSquare();
	    		break;
	    	}
	    }

		CheckStack.push(board[king_pos]); // make the king last in the stack

	    // how many pieces are checking the king
	    for(auto elem : board)
	    {
	    	if(!elem->isEmpty() && !isSameColor(elem->getPieceSquare(), king_pos) && elem->isLegalMove(king_pos) && isPathFree(elem->getPieceSquare(), king_pos) && !elem->isPinned(king_pos))
	    	{
	    		checking_piece_counter++;
	    	}
	    }
	}

	if(checking_piece_counter == 2)
	{
		chess.setCheckStack(CheckStack);
        chess.setDoubleCheck(true);
		return true;
	}

	return chess.getDoubleCheck();
}

bool Piece::causeCheck(int dest)
{
	// causes a check if not a king (cannot check opponent with your king)
	if(this->isKing() || !chess.getCheckStack().empty())
	{
		return false;
	}

	stack<Piece*> CheckStack = chess.getCheckStack();
    vector<Piece*> board = chess.getBoard();

    int king_pos;
    for(auto elem : board)
    {
    	if(elem->isKing() && !isSameColor(elem->getPieceSquare(), dest))
    	{
    		king_pos = elem->getPieceSquare();
    		break;
    	}
    }

    if(isPathFree(dest, king_pos) && board[dest]->isLegalMove(king_pos))
    {
        CheckStack.push(board[king_pos]);
        CheckStack.push(board[dest]);
        chess.setCheckStack(CheckStack);
        chess.setCheck(true);
    }

    return chess.getCheck();
}

void Piece::enPassantHandling(int src, int dest)
{
	if(this->isPawn())
	{
		this->enPassantHandling(src, dest);
	}
	else
	{
		return;
	}
}

bool Piece::isPinned(int dest)
{
	int king_pos, src = this->getPieceSquare();
	vector<Piece*> board = chess.getBoard();

	if(!this->isKing())
	{
		for(auto elem : board)
		{
			if(isSameColor(elem->getPieceSquare(), src) && elem->isKing())
			{
				king_pos = elem->getPieceSquare();
				break;
			}
		}

		for(auto elem : board)
		{
			if(!isSameColor(elem->getPieceSquare(), king_pos) && (elem->isBishop() || elem->isRook() || elem->isQueen())
			 	&& squareOfPieceInPath(elem->getPieceSquare(), king_pos) == src)
			{	
				return destInPath(src, dest, elem->getPieceSquare());
			}
		}
	}

	return false;
}

bool Piece::movedIntoCheck(int dest)
{
	if(this->isKing())
	{
		return this->movedIntoCheck(dest);
	}
	else
	{
		return false;
	}
}

bool Piece::canCastle(int dest)
{
	vector<Piece*> board = chess.getBoard();
	if(this->isKing() && board[dest]->isRook() && this->getPieceSquare()/8 == dest/8)
    	return this->canCastle(dest);
    else // not king -> cannot castle
    	return false;
}

void Piece::promotePawn(int dest)
{
	if(this->isPawn())
	{
		this->promotePawn(dest);
	}
	else
	{
		return; // do nothing
	}
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

/*****
	KING CLASS - MEMBER FUNCTIONS
 *****/

bool King::canCastle(int dest)
{
	int src = this->getPieceSquare();
	bool going_left = src > dest ? true : false;
	int increment = going_left ? -1 : 1;

	vector<Piece*> board = chess.getBoard();
	stack<Piece*> stack = chess.getCheckStack();

	if(this->getPieceMoveInfo() || board[dest]->getPieceMoveInfo() || chess.getCheck())
	{
		return false;
	}
	else
	{
	    for(auto elem : board)
	    {
	        if(!elem->isEmpty() && !elem->isSameColor(elem->getPieceSquare(), this->getPieceSquare()))
	        {
	        	// king only moves 2 squares regardless of castle direction
	            for(int i = src+increment; i != src+(2*increment); i += increment) 
	            {	
	                if(elem->isLegalMove(i))
	                {
	                	return false;
		            }
	            }
	        }
	    }

    	return this->isPathFree(src, dest);
	}
}

bool King::movedIntoCheck(int dest)
{
	vector<Piece*> board = chess.getBoard();

    for(auto elem : board)
    {
        if(elem->getPieceColor() != NEUTRAL && !isSameColor(this->getPieceSquare(), elem->getPieceSquare()))
        {
			// pawn can only attack sideways, but the board isn't updated yet so it will always be invalid move
        	if( (!elem->isPawn() && elem->isLegalMove(dest)) ||
        		(elem->isPawn() && elem->isPieceWhite() && (elem->getPieceSquare()-dest == 9 || elem->getPieceSquare()-dest == 7)) ||
        		(elem->isPawn() && elem->isPieceBlack() && (dest-elem->getPieceSquare() == 9 || dest-elem->getPieceSquare() == 7)) )
            	return true;
        }
    }

	return false;
}

bool King::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int diff = abs(src - dest);
	return (((diff == 1 || diff == 7 || diff == 8 || diff == 9) && !isSameColor(src, dest)) || ((diff == 3 || diff == 4) && this->canCastle(dest) && isSameColor(src, dest))) && !this->movedIntoCheck(dest);
}

/*****
	PAWN CLASS - MEMBER FUNCTIONS
 *****/

void Pawn::enPassantHandling(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	if(abs(src-dest) == 16 && board[dest-1]->isPawn() && !isSameColor(dest-1, dest))
	{
		board[dest-1]->setEnPassant(true);
	}
	else if(abs(src-dest) == 16 && board[dest+1]->isPawn() && !isSameColor(dest+1, dest))
	{
		board[dest+1]->setEnPassant(true);
	}
	else
	{
		// if move was made after en-passant was set, cancel en-passant abilities of all pieces
		for(auto elem : board)
		{
			if(elem->isPawn())
			{
				elem->setEnPassant(false);
			}
		}
	}
}

void Pawn::promotePawn(int dest)
{
	vector<Piece*> board = chess.getBoard();
	char piece;
	int src = this->getPieceSquare();
    while(true)
    {
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
        cout << "\nWhich Piece: Q/q | R/r | B/b | N/n? ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
        cin >> piece;
        if(piece == 'Q' || piece == 'q')
        {
            board[src] = chess.getTurn() == 2 ? new Queen(dest, 9, QUEEN, WHITE) : new Queen(dest, 9, QUEEN, BLACK);
            break;
        }
        if(piece == 'R' || piece == 'r')
        {
            board[src] = chess.getTurn() == 2 ? new Rook(dest, 5, ROOK, WHITE) : new Rook(dest, 5, ROOK, BLACK);
            break;
        }
        if(piece == 'B' || piece == 'b')
        {
            board[src] = chess.getTurn() == 2 ? new Bishop(dest, 3, BISHOP, WHITE) : new Bishop(dest, 3, BISHOP, BLACK);
            break;
        }
        if(piece == 'N' || piece == 'n')
        {
            board[src] = chess.getTurn() == 2 ? new Knight(dest, 3, KNIGHT, WHITE) : new Knight(dest, 3, KNIGHT, BLACK);
            break;
        }

        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
        cout << "Please make sure to pick correct value!" << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
    }

    chess.setBoard(board);
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
            legal = (src-dest == 8 && board[dest]->isEmpty()) || ((src-dest == 7 || src-dest == 9) && (!board[dest]->isEmpty() || getEnPassant()));
        else // black, goes down
            legal = (dest-src == 8 && board[dest]->isEmpty()) || ((dest-src == 7 || dest-src == 9) && (!board[dest]->isEmpty() || getEnPassant()));
    }
    else // cannot en-passant if you have not moved yet
    {
        if(this->isPieceWhite()) // goes up
            legal = ((src-dest == 8 || src-dest == 16) && board[dest]->isEmpty()) || ((src-dest == 7 || src-dest == 9) && !board[dest]->isEmpty());
        else // black, goes down
            legal = ((dest-src == 8 || dest-src == 16) && board[dest]->isEmpty()) || ((dest-src == 7 || dest-src == 9) && !board[dest]->isEmpty());
    }

    return legal && isPathFree(src, dest) && !isSameColor(src, dest);
}

/*****
	KNIGHT CLASS - MEMBER FUNCTIONS
 *****/

bool Knight::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return diff_row <= 2 && diff_col <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17) && !isSameColor(src, dest);
}

/*****
	BISHOP CLASS - MEMBER FUNCTIONS
 *****/

bool Bishop::isLegalMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return (diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col && isPathFree(src, dest) && !isSameColor(src, dest);
}

/*****
	ROOK CLASS - MEMBER FUNCTIONS
 *****/

bool Rook::isLegalMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	return (src_row == dest_row || src_col == dest_col) && isPathFree(src, dest) && !isSameColor(src, dest);
}

/*****
	QUEEN CLASS - MEMBER FUNCTIONS
 *****/

bool Queen::isLegalMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = abs(src - dest), diff_row = abs(src_row - dest_row), diff_col = abs(src_col - dest_col);
	return (((diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col) || (src_row == dest_row || src_col == dest_col)) && isPathFree(src, dest) && !isSameColor(src, dest);
}


/*****
	GLOBAL FUNCTIONS
 *****/

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
				board.push_back(new Rook(i, 5, ROOK, BLACK));
			}
			else if(i == 1 || i == 6) // knight
			{
				board.push_back(new Knight(i, 3, KNIGHT, BLACK)); 
			}
			else if(i == 2 || i == 5) // bishop
			{
				board.push_back(new Bishop(i, 3, BISHOP, BLACK)); 
			}
			else if(i == 3) // queen
			{
				board.push_back(new Queen(i, 9, QUEEN, BLACK));
			}
			else if(i == 4) // king
			{
				board.push_back(new King(i, 10, KING, BLACK));
			}
			else // pawn
			{
				board.push_back(new Pawn(i, 1, PAWN, BLACK));
			}
		}
		else if(board_size/4 <= i && i < board_size*3/4) // blank squares
		{	
			board.push_back(new Empty(i, 0, EMPTY, NEUTRAL));
		}
		else // white
		{
			if(i == 56 || i == 63) // rook
			{
				board.push_back(new Rook(i, 5, ROOK, WHITE));
			}
			else if(i == 57 || i == 62) // knight
			{
				board.push_back(new Knight(i, 3, KNIGHT, WHITE)); 
			}
			else if(i == 58 || i == 61) // bishop
			{
				board.push_back(new Bishop(i, 3, BISHOP, WHITE));
			}
			else if(i == 59) // queen
			{
				board.push_back(new Queen(i, 9, QUEEN, WHITE));
			}
			else if(i == 60) // king
			{
				board.push_back(new King(i, 10, KING, WHITE));
			}
			else // pawn
			{
				board.push_back(new Pawn(i, 1, PAWN, WHITE));
			}
		}
	}

	chess.setBoard(board);

	printBoard(board);

	cout << "___________________________________________________" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CYAN);
	cout << "\n            White's move" << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
}

// Print the current board position
void printBoard(const vector<Piece*> & board)
{
	char piece_char;
	char ranks[8] = {'H', 'G', 'F', 'E', 'D', 'C', 'B', 'A'};
	
	int count = 0;
	for(auto elem : board)
	{
		if(count == 0)
		{
			cout << "  +---+---+---+---+---+---+---+---+" << endl;
			cout << ranks[count/8] << " | ";
		}
		else if(count % 8 == 0 && count > 0)
		{
			cout << "  +---+---+---+---+---+---+---+---+" << endl;
			cout << ranks[count/8] << " | ";
		}

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
				piece_char = ' ';
		}

		if(!islower(piece_char)) // WHITE
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), GREEN);
			cout << std::left << std::setw(2) << piece_char;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
		}
		else // BLACK
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), RED);
			cout << std::left << std::setw(2) << piece_char;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
		}
		
		cout << "| ";

		// go to next row if reached last column
		if(count % 8 == 7)
			cout << endl;
		if(count == 63)
		{
			cout << "  +---+---+---+---+---+---+---+---+" << endl;
			cout << "    1   2   3   4   5   6   7   8" << endl;
		}
		count++;
	}
}
