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
				if((move_up <= 63 && elem->isPossibleMove(move_up) && !elem->isPinned(move_up)) || (move_down >= 0 && elem->isPossibleMove(move_down) && !elem->isPinned(move_down)))
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
		if(doubleCheckPieceIterator(king->getPieceSquare()) || king->getPieceColor() != current_turn)
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
		if(singleCheckPieceIterator(piece->getPieceSquare(), king->getPieceSquare()) || piece->getPieceColor() == current_turn)
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

bool Chess::doubleCheckPieceIterator(int dest)
{
	int king_moves[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

	// can king move out of check?
    for(auto move : king_moves)
    {
    	if(dest+move >= 0 && dest + move <=63 && board[dest]->isPossibleMove(dest + move) && !board[dest]->movedIntoCheck(dest + move))
    	{
    		return false;
    	}
    }
    	
    return true; // no legal moves found ? true : false

}

void Chess::makeMove(string src, string dest)
{
	int src_int, dest_int;

	// src
	if(islower(src[0])) // letter
	{
		// 				file					rank
		src_int = (int(src[0]) - 97) + (8 - (int(src[1]) - 48))*8;
	}
	else // number
	{
		// 				file					rank
		src_int = (int(src[0]) - 65) + (8 - (int(src[1]) - 48))*8;
	}

	// dest
	if(islower(dest[0])) // letter
	{
		// 				file					rank
		dest_int = (int(dest[0]) - 97) + (8 - (int(dest[1]) - 48))*8;
	}
	else // number
	{
		// 				file					rank
		dest_int = (int(dest[0]) - 65) + (8 - (int(dest[1]) - 48))*8;
	}

	this->makeMove(src_int, dest_int); // call the coordinate version
}

void Chess::makeMove(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
	stack<Piece*> CheckStack = chess.getCheckStack();
	Piece *king, *piece, *undo_piece; // "undo_piece" needed for attacking moves that are illegal (to restore the piece that goes missing)
	bool undo_moveInfo;
 	
    if(0 <= src && src <= 63 && 0 <= dest && dest <= 63 && dest != src &&
       board[src]->isPossibleMove(dest) && board[src]->getPieceColor() == chess.getTurn() && !board[src]->isPinned(dest))
    {	
        // pawn promotion
        if(board[src]->isPawn() && (dest/8 == 0 || dest/8 == 7))
        {
            board[src]->promotePawn(dest);
        }

		undo_piece = board[dest];
		undo_moveInfo = board[src]->getPieceMoveInfo();

        this->makeMoveForType(src, dest);

        // when in double check you must move the king
	    if(chess.getDoubleCheck())
	    {      
    		king = CheckStack.top();
			CheckStack.pop();

			for(auto elem : chess.getBoard())
			{					
				if(elem->isPathFree(king->getPieceSquare()) && elem->isPossibleMove(king->getPieceSquare()))
				{
					makeMoveForType(dest, src); // undo the move
					board = chess.getBoard();
					board[dest] = undo_piece;
					board[src]->setPieceMoveInfo(undo_moveInfo);
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

			if(piece->isPossibleMove(king->getPieceSquare()) && piece->isPathFree(king->getPieceSquare()))
			{
				makeMoveForType(dest, src); // undo the move
				board = chess.getBoard();
				board[dest] = undo_piece;
				board[src]->setPieceMoveInfo(undo_moveInfo);
				chess.setBoard(board);

				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
				cout << "You are in check! Try again..." << endl;
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
				return;
			}

			chess.setCheckStack(CheckStack);
			chess.setCheck(false);
        }

        board = chess.getBoard();
        board[dest]->enPassantHandling(src); // en-passant checking/updating

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

		chess.setTurn(chess.getTurn() == 2 ? BLACK : WHITE);
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

bool Piece::isPathFree(int dest)
{
	int increment;

	if(this->isKnight())
	{
		return true;
	}
	else
	{
		increment = incrementChoice(this->getPieceSquare(), dest);
    	return increment > 0 ? pathIterator(this->getPieceSquare(), dest, increment) : false;
	}
    
}

/*****
	CHESS CLASS - HELPER FUNCTIONS
 *****/

void Chess::makeMoveForType(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();

	// castling move
	if(board[src]->canCastle(dest) && (std::abs(src - dest) == 3 || std::abs(src - dest) == 4))
	{
	    // note that the pieces are moved
	    board[src]->setPieceMoveInfo(true);
	    board[dest]->setPieceMoveInfo(true);

	    if(std::abs(src - dest) == 3) // king side castle
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

	    else // std::abs(src-dest) == 4 -> queen side castle
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
		if(std::abs(src-dest) == 7)
		{
			board[src+1] = new Empty(src+1, 0, EMPTY, NEUTRAL);
		}
		else if(std::abs(src-dest) == 9)
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

bool Chess::singleCheckPieceIterator(int src, int dest)
{
	vector<Piece*> board = chess.getBoard();
    int increment;
    int king_moves[8] = {-9, -8, -7, -1, 1, 7, 8, 9};

    // can a piece defend the king from check?
    if(!board[src]->isKnight())
    {
    	increment = incrementChoice(src, dest); // passed by reference
	    for(auto elem : chess.getBoard())
	    {
	        if(elem->isSameColor(dest) && elem->getPieceSquare() != dest)
	        {
	            for(int i = std::min(src, dest)+increment; i <= std::max(src, dest); i += increment)
	            {	
	                if(elem->isPossibleMove(i))
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
	        if(elem->isSameColor(dest) && elem->getPieceSquare() != dest && elem->isPossibleMove(src))
	        {
	            return false;
	        }
	    }
    }

    // can king move out of check?
    for(auto move : king_moves)
    {
    	if(dest + move >= 0 && dest + move <=63 && board[dest]->isPossibleMove(dest + move) && !board[dest]->movedIntoCheck(dest + move))
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
		for(int i = std::min(src, dest)+increment; i < std::max(src, dest); i += increment)
		{
			if(!board[i]->isEmpty())
			{
				pieces_in_path.push_back(i);
			}
		}
	}

	return pieces_in_path.size() == 1 ? pieces_in_path[0] : -1;
}


int Chess::incrementChoice(int src, int dest)
{
    if(src/8 == dest/8) // row path
    {
        return 1;
    }
    else if(src%8 == dest%8) // column path
    {
        return 8;
    }
    else if(std::abs(src/8 - dest/8) == std::abs(src%8 - dest%8)) // diagonal path
    {
        return std::abs(src - dest) % 7 == 0 ? 7 : 9;
    }
    else
    {
    	return 0;
    }
}

bool Chess::pathIterator(int src, int dest, int increment)
{
	vector<Piece*> board = chess.getBoard();

	for(int i = std::min(src, dest)+increment; i < std::max(src, dest); i+=increment)
    {
        if(!board[i]->isEmpty())
            return false;
    }
    return true;
}

bool Chess::destInPath(int src, int dest, int pin)
{
	int increment_dest = incrementChoice(src, dest); // passed by reference
	int increment_pin = incrementChoice(src, pin);

	return !(increment_dest != 0 && increment_pin != 0 && increment_dest == increment_pin && std::min(src, dest) + increment_pin <= std::max(src, dest));
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
	    	if(elem->isKing() && !elem->isSameColor(dest))
	    	{
	    		king_pos = elem->getPieceSquare();
	    		break;
	    	}
	    }

		CheckStack.push(board[king_pos]); // make the king last in the stack

	    // how many pieces are checking the king
	    for(auto elem : board)
	    {
	    	if(!elem->isEmpty() && !elem->isSameColor(king_pos) && elem->isPossibleMove(king_pos) && elem->isPathFree(king_pos) && !elem->isPinned(king_pos))
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
	stack<Piece*> CheckStack = chess.getCheckStack();
    vector<Piece*> board = chess.getBoard();
    int king_pos;

	// causes a check if not a king (cannot check opponent with your king)
	if(this->isKing() || !chess.getCheckStack().empty())
	{
		return false;
	}

    for(auto elem : board)
    {
    	if(elem->isKing() && !elem->isSameColor(dest))
    	{
    		king_pos = elem->getPieceSquare();
    		break;
    	}
    }

    if(board[dest]->isPathFree(king_pos) && board[dest]->isPossibleMove(king_pos))
    {
        CheckStack.push(board[king_pos]);
        CheckStack.push(board[dest]);
        chess.setCheckStack(CheckStack);
        chess.setCheck(true);
    }

    return chess.getCheck();
}

void Piece::enPassantHandling(int src)
{
	if(this->isPawn())
		this->enPassantHandling(src);
}

bool Piece::isPinned(int dest)
{
	int king_pos, src = this->getPieceSquare();
	vector<Piece*> board = chess.getBoard();

	if(!this->isKing())
	{
		for(auto elem : board)
		{
			if(elem->isSameColor(src) && elem->isKing())
			{
				king_pos = elem->getPieceSquare();
				break;
			}
		}

		for(auto elem : board)
		{
			if(!elem->isSameColor(king_pos) && (elem->isBishop() || elem->isRook() || elem->isQueen())
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
	return this->isKing() ? this->movedIntoCheck(dest) : false;
}

bool Piece::canCastle(int dest)
{
	return this->isKing() ? this->canCastle(dest) : false;
}

void Piece::promotePawn(int dest)
{
	if(this->isPawn())
		this->promotePawn(dest);
}

bool Piece::isSameColor(int dest)
{
	vector<Piece*> board = chess.getBoard();

	// cannot use this->getPieceColor() here since the board might be updated
	return board[this->getPieceSquare()]->getPieceColor() == board[dest]->getPieceColor();
}

bool Piece::isPossibleMove(int dest)
{
	return !this->isEmpty() ? this->isPossibleMove(dest) : false;
}

/*****
	KING CLASS - MEMBER FUNCTIONS
 *****/

bool King::canCastle(int dest)
{
	int src = this->getPieceSquare();
	int increment = src > dest ? -1 : 1;

	vector<Piece*> board = chess.getBoard();
	stack<Piece*> stack = chess.getCheckStack();

	if(this->getPieceMoveInfo() || board[dest]->getPieceMoveInfo() || chess.getCheck() || !board[dest]->isRook() || this->getPieceSquare()/8 != dest/8)
	{
		return false;
	}
	else
	{
	    for(auto elem : board)
	    {
	        if(!elem->isEmpty() && !elem->isSameColor(src))
	        {
	        	// king only moves 2 squares regardless of castle direction
	            for(int square = src+increment; square != src+(2*increment); square += increment) 
	            {	
	                if(elem->isPossibleMove(square))
	                {
	                	return false;
		            }
	            }
	        }
	    }

    	return this->isPathFree(dest);
	}
}

bool King::movedIntoCheck(int dest)
{
	vector<Piece*> board = chess.getBoard();

    for(auto elem : board)
    {
        if(elem->getPieceColor() != NEUTRAL && !this->isSameColor(elem->getPieceSquare()))
        {
			// pawn can only attack sideways, but the board isn't updated yet so it will always be invalid move
        	if( (!elem->isPawn() && elem->isPossibleMove(dest)) ||
        		(elem->isPawn() && elem->isPieceWhite() && (elem->getPieceSquare()-dest == 9 || elem->getPieceSquare()-dest == 7)) ||
        		(elem->isPawn() && elem->isPieceBlack() && (dest-elem->getPieceSquare() == 9 || dest-elem->getPieceSquare() == 7)) )
            	return true;
        }
    }

	return false;
}

bool King::isPossibleMove(int dest)
{
	vector<Piece*> board = chess.getBoard();

	int src = this->getPieceSquare();
	int diff = std::abs(src - dest);
	return (((diff == 1 || diff == 7 || diff == 8 || diff == 9) && !this->isSameColor(dest)) || ((diff == 3 || diff == 4) && this->canCastle(dest) && this->isSameColor(dest))) && !this->movedIntoCheck(dest);
}

/*****
	PAWN CLASS - MEMBER FUNCTIONS
 *****/

void Pawn::enPassantHandling(int src)
{
	vector<Piece*> board = chess.getBoard();
	int dest = this->getPieceSquare();
	
	if(std::abs(src-dest) == 16 && board[dest-1]->isPawn() && !board[dest-1]->isSameColor(dest))
	{
		cout << "here" << endl;
		board[dest-1]->setEnPassant(true);
	}
	else if(std::abs(src-dest) == 16 && board[dest+1]->isPawn() && !board[dest+1]->isSameColor(dest))
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

bool Pawn::isPossibleMove(int dest)
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

    return legal && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*****
	KNIGHT CLASS - MEMBER FUNCTIONS
 *****/

bool Knight::isPossibleMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = std::abs(src - dest), diff_row = std::abs(src_row - dest_row), diff_col = std::abs(src_col - dest_col);
	return diff_row <= 2 && diff_col <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17) && !this->isSameColor(dest);
}

/*****
	BISHOP CLASS - MEMBER FUNCTIONS
 *****/

bool Bishop::isPossibleMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = std::abs(src - dest), diff_row = std::abs(src_row - dest_row), diff_col = std::abs(src_col - dest_col);
	return (diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*****
	ROOK CLASS - MEMBER FUNCTIONS
 *****/

bool Rook::isPossibleMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	return (src_row == dest_row || src_col == dest_col) && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*****
	QUEEN CLASS - MEMBER FUNCTIONS
 *****/

bool Queen::isPossibleMove(int dest)
{
	int src = this->getPieceSquare();
	int src_row = src/8, src_col = src%8, dest_row = dest/8, dest_col = dest%8;
	int diff = std::abs(src - dest), diff_row = std::abs(src_row - dest_row), diff_col = std::abs(src_col - dest_col);
	return (((diff % 7 == 0 || diff % 9 == 0) && diff_row == diff_col) || (src_row == dest_row || src_col == dest_col)) && this->isPathFree(dest) && !this->isSameColor(dest);
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
	char ranks[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
	
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
			cout << "    A   B   C   D   E   F   G   H" << endl;
		}
		count++;
	}
}
