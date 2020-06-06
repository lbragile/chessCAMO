/********************************************************************************************/  
/*                          Title:           chess.cpp                                      */
/*                          Author:          Lior Bragilevsky                               */
/*                          Related Files:   chess.h                                        */
/*                          Project:         chessCAMO                                      */
/*                          Version:         1.0                                            */
/*                          Last Revision:   June 6th, 2020                                 */
/********************************************************************************************/ 

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

/*************************************************************************************/
/*                              LOCAL FUNCTIONS / OBJECTS                            */
/*************************************************************************************/
namespace
{
    // Description:     Determines if a destination square is in the pinning path
    //                  as sometimes you can simply move the pinned piece closer to the 
    //                  pinning piece.
    // Pre-condition:   'src'       - source square of pinned piece
    //                  'dest'      - destination square of pinned piece
    //                  'pin'       - source square of pinning piece    
    // Post-condition:  Allows the move (returns false) if piece can move closer to pinning
    //                  piece, else move is invalid (returns true)
    bool destNotInPinPath(int src, int dest, int pin);

    // Description:     Determines if the source and destination squares are in the same column
    // Pre-condition:   'src'       - source square of piece
    //                  'dest'      - destination square of piece   
    // Post-condition:  Returns true if the source and destination are in the same column,
    //                  else retuns false.
    bool sameCol(int src, int dest);

    // similar to sameCol(.), just for rows
    bool sameRow(int src, int dest);

    // similar to sameCol(.), just for diagonals
    bool sameDiag(int src, int dest);

    // Description:     Used to determine the coordinate of a pinned piece.
    // Pre-condition:   'chess'     - object is created
    //                  'src'       - source square of pinning piece
    //                  'dest'      - destination square of pinning piece
    // Post-condition:  If only 1 piece is in the path from 'src' to 'dest', return its coordinates
    //                  else, return -1 to indicate that there is either no piece or multiple
    //                  pieces in the path.
    int squareOfPieceInPath(int src, int dest);

    // Description:     Used to determine the increment to use when moving a piece from 'src' to 'dest'.
    // Pre-condition:   'src'       - source square of piece
    //                  'dest'      - destination square of piece
    // Post-condition:  Returns either 1 if moving in same row, 7 if moving in diagonal to the right,
    //                  8 if moving in same column, 9 if moving in diagonal to the left. If move does 
    //                  not correspond to one of the pieces, returns 0.
    int incrementChoice(int src, int dest);

    // Description:     Used to determine the coordinate of a pinned piece.
    // Pre-condition:   'chess'     - object is created
    //                  'src'       - source square of pinning piece
    //                  'board'     - initialize board representing the current position
    //                  'enemy'     - true if king color differs from piece color, else false
    // Post-condition:  Returns the coordinate position of the king, based on the current board
    //                  representation and color determined by 'enemy'. If king is not found
    //                  returns -1.
    int findKingPos(int src, const vector<Piece*> & board, bool enemy); 
} // unnamed namespace (makes these functions local to this implementation file)

/*************************************************************************************/
/*                              CHESS CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
// Destructor
Chess::~Chess()
{
    vector<Piece*> board = this->getBoard();
    while(!board.empty())
    {
        board.pop_back();
    }
}

// Description:     Moves a piece on the board from 'src' to 'dest' if conditions
//                  for a legal move are met
// Pre-condition:   'chess'     - object is created
//                  'src'       - source square (piece's current location)
//                  'dest'      - destination square (piece's ending location)
// Post-condition:  The pieces at 'src' and 'dest' positions are swapped.
//                  If needed (attacking, castling, etc.) an empty square is made.
//                  The board's state is updated to indicate that the move occured.
//                  On failure, an error message is printed and user is asked to retry.
void Chess::makeMove(int src, int dest)
{
    vector<Piece*> board = this->getBoard();
    stack<Piece*> CheckStack = this->getCheckStack();
    Piece *king, *piece, *undo_piece; // "undo_piece" needed for attacking moves that are illegal (to restore the piece that goes missing)
    bool undo_moveInfo;
    
    if(0 <= src && src <= 63 && board[src]->isLegalMove(dest) && board[src]->getPieceColor() == this->getTurn())
    {   
        // pawn promotion
        if(board[src]->isPawn() && (dest/8 == 0 || dest/8 == 7))
        {
            board[src]->promotePawn(dest);
        }

        // store piece and it's information in case move fails (to restore board representation from previous move)
        undo_piece = board[dest];
        undo_moveInfo = board[src]->getPieceMoveInfo();

        this->makeMoveForType(src, dest);

        // when in double check you must move the king
        if(this->getDoubleCheck())
        {      
            king = CheckStack.top();
            CheckStack.pop();

            for(auto elem : this->getBoard()) // must always update the board as it is possible an undo happened
            {                   
                if(this->undoMove(src, dest, king, elem, undo_piece, undo_moveInfo, "double"))
                    return;
            }

            // if here, did not return so set the appropriate member variables
            this->setCheckStack(CheckStack);
            this->setDoubleCheck(false);
        }

        else if(this->getCheck())
        {
            piece = CheckStack.top();
            CheckStack.pop();
            king = CheckStack.top();
            CheckStack.pop();

            if(this->undoMove(src, dest, king, piece, undo_piece, undo_moveInfo, "single"))
                return;

            // if here, did not return so set the appropriate member variables
            this->setCheckStack(CheckStack);
            this->setCheck(false);
        }

        board = this->getBoard();
        board[dest]->enPassantHandling(src); // en-passant checking/updating

        // did the move cause a check?
        if(board[src]->causeCheck(dest) && !board[src]->causeDoubleCheck(dest)) 
        {
            this->isCheckmate("single"); // check for checkmates
        }

        // did the move cause a double check?
        if(!board[src]->causeCheck(dest) && board[src]->causeDoubleCheck(dest)) 
        {
            this->isCheckmate("double"); // check for checkmates
        }

        // check for stalemate
        if(this->isStalemate()) 
        {
            this->handleStalemate();
        }

        this->handleChangeTurn();
    }
    else
    {
        chessCAMO::printBoard(board);
        chessCAMO::printMessage("\nInvalid move! Try again...\n", YELLOW);
    }
}

// same as above, but converts the string into it's coordinate (integer) and called above function.
void Chess::makeMove(string src, string dest)
{
    int src_int, dest_int;

    /************ src **********/
    if(islower(src[0])) // letter
    {
        src_int = (int(src[0]) - 97) + (8 - (int(src[1]) - 48))*8; // file + rank
    }
    else // number
    {
        src_int = (int(src[0]) - 65) + (8 - (int(src[1]) - 48))*8; // file + rank
    }

    /************ dest **********/
    if(islower(dest[0])) // letter
    {
        dest_int = (int(dest[0]) - 97) + (8 - (int(dest[1]) - 48))*8; // file + rank
    }
    else // number
    {
        dest_int = (int(dest[0]) - 65) + (8 - (int(dest[1]) - 48))*8; // file + rank
    }

    this->makeMove(src_int, dest_int); // call the coordinate version
}

// Description:     Decide if a move caused a checkmate
// Pre-condition:   'chess'     - object is created
//                  'check_type'    - string representing the checking type ("single" or "double")      
// Post-condition:  If board's state is in checkmate, calls handleCheckmate() to print messages
//                  to console indicating the winner. Else, game continues as usual.
void Chess::isCheckmate(string check_type)
{
    stack<Piece*> CheckStack = this->getCheckStack();
    Piece *king, *piece;

    if(check_type == "double") // this type of check requires the king to move
    {
        king = CheckStack.top();

        // checkmate due to a double check
        if(doubleCheckPieceIterator(king) || king->getPieceColor() != this->switchTurn())
            handleCheckmate();
        else // was not checkmate so can state that it is double check
            chessCAMO::printMessage("\n            Double Check!\n\n", CYAN); 
    }   

    else if(check_type == "single") // CheckStack.top()->getPieceColor() != this->switchTurn()
    {
        // will not be set, so next time this will be identical
        piece = CheckStack.top();
        CheckStack.pop();
        king = CheckStack.top();

        // checkmate due to a single piece
        if(singleCheckPieceIterator(piece, king) || piece->getPieceColor() == this->switchTurn())
            handleCheckmate();
        else // was not checkmate so can state that it is check
            chessCAMO::printMessage("\n                Check!\n\n", CYAN); 
    }
}

// Description:     Decide if a move caused a stalemate
// Pre-condition:   'chess'     - object is created     
// Post-condition:  If board's state is in stalemate, calls handleStalemate() to print messages
//                  to console indicating that game is drawn. Else, game continues as usual.
bool Chess::isStalemate()
{
    vector<Piece*> board = this->getBoard();
    int possible_moves[22] = {1, 6, 7, 8, 9, 10, 14, 15, 16, 17, 18, 21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63};
    
    for(auto elem : board)
    {
        if(elem->getPieceColor() == this->switchTurn())
        {
            for(int move : possible_moves)
            {
                if(elem->isLegalMove(elem->getPieceSquare() + move) || elem->isLegalMove(elem->getPieceSquare() - move))
                {
                    return false; // at least one piece could move on the board
                }
            }
        }
    }

    return true; // no piece on the board has a legal move
}

/*************************************************************************************/
/*                              CHESS CLASS - HELPER FUNCTIONS                       */
/*************************************************************************************/

// Description:     A move can be one of: attack, castle, en-passant, regular
// Pre-condition:   'chess'     - object is created
//                  'src'       - valid input source square
//                  'dest'      - valid input destination square (move is legal)        
// Post-condition:  Swaps the pieces on the board according to 'src' and 'dest' and proper
//                  chess rules, using pieceSwap(.). If a new empty square must be created,
//                  this is handled. Returns board representation with the made move.
void Chess::makeMoveForType(int src, int dest)
{
    vector<Piece*> board = this->getBoard();

    // castling move
    if(board[src]->canCastle(dest) && (std::abs(src - dest) == 3 || std::abs(src - dest) == 4))
    {
        // note that the pieces are moved
        board[src]->setPieceMoveInfo(true);
        board[dest]->setPieceMoveInfo(true);

        if(std::abs(src - dest) == 3) // king side castle
        {
            this->pieceSwap(src, src + 2, board); // king move
            this->pieceSwap(dest, dest - 2, board); // rook move
        }

        else // std::abs(src-dest) == 4 -> queen side castle
        {
            this->pieceSwap(src, src - 2, board); // king move
            this->pieceSwap(dest, dest + 3, board); // rook move
        }
    }

    // en-passant move
    else if(board[src]->isPawn() && board[src]->getEnPassant() && !sameCol(src, dest))
    {
        this->pieceSwap(src, dest, board);

        // delete the pawn that caused en-passant (make it empty square)
        if(std::abs(src-dest) == 7)
        {
            board[src+1] = new Empty(src+1, EMPTY, NEUTRAL);
        }
        else if(std::abs(src-dest) == 9)
        {
            board[src-1] = new Empty(src-1, EMPTY, NEUTRAL);
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
            this->pieceSwap(src, dest, board);
        }

        // Attacking Move
        else
        {
            // dest
            board[dest] = board[src];
            board[dest]->setPieceSquare(dest);

            // src
            board[src] = new Empty(src, EMPTY, NEUTRAL);
            board[src]->setPieceSquare(src);
        }
    }

    this->setBoard(board);
}

// Description:     Used in makeMoveForType(.) to swap pieces on the board
// Pre-condition:   'chess'     - object is created
//                  'src'       - valid input source square
//                  'dest'      - valid input destination square (move is legal)
//                  'board'     - valid board representation        
// Post-condition:  Swaps the pieces on the board according to 'src' and 'dest'.
void Chess::pieceSwap(int src, int dest, vector<Piece*> & board)
{
    board[src]->setPieceSquare(dest);
    board[dest]->setPieceSquare(src);
    std::swap(board[src], board[dest]);
}

// Description:     Indicates who will move next via a message to console
// Pre-condition:   'chess'     - object is created 
// Post-condition:  Player turn is switched, board is printed, and message is displayed
//                  if game is not over to indicate whose turn it is.
void Chess::handleChangeTurn()
{
    this->setTurn(this->switchTurn());
    chessCAMO::printBoard(this->getBoard());
    if(!this->getCheckmate() && !this->getStalemate())
    {
        cout << "___________________________________________________" << endl;
        if(this->getTurn() == WHITE)
            chessCAMO::printMessage("\n            White's move\n", CYAN);
        else
            chessCAMO::printMessage("\n            Black's move\n", CYAN);
    }
}

// Description:     Indicates which player won by checkmate via a message to console
// Pre-condition:   'chess'     - object is created
//                  A move was made (cannot checkmate in less than 2 moves in theory)   
// Post-condition:  global object's checkmate state is set to true (to end the algorithm)
void Chess::handleCheckmate()
{
    string message = this->getTurn() == WHITE ? "\n      White won by Checkmate!\n\n" : "\n      Black won by Checkmate!\n\n";
    chessCAMO::printMessage(message, CYAN);
    this->setCheckmate(true);
}

// Description:     Indicates the game is drawn via a message to console
// Pre-condition:   'chess'     - object is created
//                  A move was made
// Post-condition:  global object's stalemate state is set to true (to end the algorithm)
void Chess::handleStalemate()
{
    string message = this->getTurn() != WHITE ? "\nWhite has no moves -> Game is Drawn!\n\n" : "\nBlack has no moves -> Game is Drawn!\n\n";
    chessCAMO::printMessage(message, CYAN);
    this->setStalemate(true);
}

// Description:     After a move is made, can undo it if move was invalid and return to
//                  previous board state
// Pre-condition:   'chess'         - object is created
//                  'src'           - source square of piece prior to current board state
//                  'dest'          - destination square of piece prior to current board state
//                  'king'          - king that is being attacked currently
//                  'piece'         - piece that is attacking the king currently
//                  'undo_piece'    - if move fails, this is the piece that was moved previously
//                  'undo_moveInfo' - if move fails, this is the piece's move information
//                  'check_type'    - "single" or "double" check
// Post-condition:  If after move, the 'king' is still in check (single or double) or the move was
//                  invalid, output warning message and undo the move. Else, continue the game
//                  without undoing the move.
bool Chess::undoMove(int src, int dest, Piece* king, Piece* piece, Piece* undo_piece, bool undo_moveInfo, string check_type)
{
    vector<Piece*> board = this->getBoard();

    if(piece->isPossibleMove(king->getPieceSquare()) && piece->isPathFree(king->getPieceSquare()))
    {
        makeMoveForType(dest, src); // undo the move
        board = this->getBoard();
        board[dest] = undo_piece;
        board[src]->setPieceMoveInfo(undo_moveInfo);
        this->setBoard(board);

        if(check_type.compare("double") == 0)
        {
            chessCAMO::printBoard(board);
            chessCAMO::printMessage("You are in double check! Try again...\n", YELLOW);
        }
        else if(check_type.compare("single") == 0)
        {
            chessCAMO::printBoard(board);
            chessCAMO::printMessage("You are in check! Try again...\n", YELLOW);
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

// Description:     If in a single check, see if piece can defend the king, capture attacking piece,
//                  or move the king out of check - used in isCheckmate("single")
// Pre-condition:   'chess'         - object is created
//                  'king'          - king that is being attacked currently
//                  'piece'         - piece that is attacking the king currently
// Post-condition:  If no legal moves found return true (checkmate), else return false and make the move
bool Chess::singleCheckPieceIterator(Piece* piece, Piece* king)
{
    vector<Piece*> board = this->getBoard();
    int increment, src = piece->getPieceSquare(), dest = king->getPieceSquare();

    // can a piece defend the king from check?
    if(!board[src]->isKnight())
    {
        increment = incrementChoice(src, dest);
        for(auto elem : board)
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
        for(auto elem : board)
        {
            if(elem->isSameColor(dest) && elem->getPieceSquare() != dest && elem->isPossibleMove(src))
            {
                return false;
            }
        }
    }

    // can king move out of check?
    return this->doubleCheckPieceIterator(king);
}

// Description:     If in a double check, see if the king can move out of check as this is the only
//                  valid move option. Used in isCheckmate("double")
// Pre-condition:   'chess'         - object is created
//                  'king'          - king that is being attacked currently
// Post-condition:  If no legal moves found return true (checkmate), else return false and make the move
bool Chess::doubleCheckPieceIterator(Piece* king)
{
    int dest = king->getPieceSquare();
    int king_moves[4] = {1, 7, 8, 9};

    // can king move out of check?
    for(auto move : king_moves)
    {
        if(board[dest]->isLegalMove(dest + move) || board[dest]->isLegalMove(dest - move))
        {
            return false;
        }
    }
        
    return true; // no legal moves found ? true : false
}

// Description:     Decides whose turn it is currently and updates the private member variable accordingly
// Pre-condition:   'chess'         - object is created
// Post-condition:  'chess.turn' is set to the correct player
pieceColor Chess::switchTurn()
{
    return this->getTurn() == WHITE ? BLACK : WHITE;
}

/*************************************************************************************/
/*                              PIECE CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
bool Piece::isSameColor(int dest)
{
    vector<Piece*> board = chess.getBoard();

    // cannot use this->getPieceColor() here since the board might be updated
    return board[this->getPieceSquare()]->getPieceColor() == board[dest]->getPieceColor();
}

bool Piece::isPinned(int dest)
{
    int king_pos, src = this->getPieceSquare();
    vector<Piece*> board = chess.getBoard();

    if(!this->isKing())
    {
        king_pos = findKingPos(src, board, false); // same color king position

        for(auto elem : board)
        {
            if(!elem->isSameColor(king_pos) && (elem->isBishop() || elem->isRook() || elem->isQueen())
                && squareOfPieceInPath(elem->getPieceSquare(), king_pos) == src)
            {   
                return destNotInPinPath(src, dest, elem->getPieceSquare());
            }
        }
    }

    return false;
}

bool Piece::isPathFree(int dest)
{
    int increment, src = this->getPieceSquare();
    vector<Piece*> board = chess.getBoard();

    if(this->isKnight())
    {
        return true;
    }
    else
    {
        increment = incrementChoice(src, dest);
        if(increment > 0)
        {
            for(int i = std::min(src, dest)+increment; i < std::max(src, dest); i+=increment)
            {
                if(!board[i]->isEmpty())
                    return false;
            }

            return true;
        }
        else
        {
            return false;
        }
    } 
}

bool Piece::isLegalMove(int dest)
{
    int src = this->getPieceSquare();
    if(0 <= dest && dest <= 63 && src != dest)
    {
        if(!this->isKing())
            return this->isPossibleMove(dest) && !this->isPinned(dest);
        else
            return this->isPossibleMove(dest) && !this->movedIntoCheck(dest);
    }
    else
        return false;
}

bool Piece::causeCheck(int dest)
{
    stack<Piece*> CheckStack = chess.getCheckStack();
    vector<Piece*> board = chess.getBoard();
    
    // causes a check if not a king (cannot check opponent with your king)
    if(this->isKing() || !chess.getCheckStack().empty())
    {
        return false;
    }
    else
    {
        int king_pos = findKingPos(dest, board, true); // opposite color king position

        if(board[dest]->isPathFree(king_pos) && board[dest]->isPossibleMove(king_pos))
        {
            CheckStack.push(board[king_pos]);
            CheckStack.push(board[dest]);
            chess.setCheckStack(CheckStack);
            chess.setCheck(true);
        }
    } 

    return chess.getCheck();
}

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
        king_pos = findKingPos(dest, board, true); // opposite color king position

        CheckStack.push(board[king_pos]); // make the king last in the stack

        // how many pieces are checking the king
        for(auto elem : board)
        {
            if(elem->isLegalMove(king_pos) && !elem->isSameColor(king_pos))
            {
                checking_piece_counter++;
            }
        }

        if(checking_piece_counter == 2)
        {
            chess.setCheckStack(CheckStack);
            chess.setDoubleCheck(true);
            return true;
        }
    }   

    return chess.getDoubleCheck();
}

bool Piece::isPossibleMove(int dest)
{
    return !this->isEmpty() ? this->isPossibleMove(dest) : false;
}

bool Piece::canCastle(int dest)
{
    return this->isKing() ? this->canCastle(dest) : false;
}

void Piece::promotePawn(int dest)
{
    if(this->isPawn()) {this->promotePawn(dest);}
}

bool Piece::movedIntoCheck(int dest)
{
    return this->isKing() ? this->movedIntoCheck(dest) : false;
}

void Piece::enPassantHandling(int src)
{
    if(this->isPawn()) {this->enPassantHandling(src);}
}


/*************************************************************************************/
/*                              PAWN CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
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

void Pawn::enPassantHandling(int src)
{
    vector<Piece*> board = chess.getBoard();
    int dest = this->getPieceSquare();

    if(std::abs(src-dest) == 16 && board[dest-1]->isPawn() && !board[dest-1]->isSameColor(dest))
    {
        board[dest-1]->setEnPassant(true);
    }
    else if(std::abs(src-dest) == 16 && board[dest+1]->isPawn() && !board[dest+1]->isSameColor(dest))
    {
        board[dest+1]->setEnPassant(true);
    }
    else
    {
        // if move was made after en-passant was set, cancel en-passant abilities of all pieces
        for(auto elem : board) {elem->setEnPassant(false);}
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
            board[src] = chess.getTurn() == WHITE ? new Queen(dest, QUEEN, WHITE) : new Queen(dest, QUEEN, BLACK);
            break;
        }
        if(piece == 'R' || piece == 'r')
        {
            board[src] = chess.getTurn() == WHITE ? new Rook(dest, ROOK, WHITE) : new Rook(dest, ROOK, BLACK);
            break;
        }
        if(piece == 'B' || piece == 'b')
        {
            board[src] = chess.getTurn() == WHITE ? new Bishop(dest, BISHOP, WHITE) : new Bishop(dest, BISHOP, BLACK);
            break;
        }
        if(piece == 'N' || piece == 'n')
        {
            board[src] = chess.getTurn() == WHITE ? new Knight(dest, KNIGHT, WHITE) : new Knight(dest, KNIGHT, BLACK);
            break;
        }

        chessCAMO::printMessage("Please make sure to pick correct value!\n", YELLOW);
    }

    chess.setBoard(board);
}

/*************************************************************************************/
/*                              KNIGHT CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
bool Knight::isPossibleMove(int dest)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return std::abs(src/8 - dest/8) <= 2 && std::abs(src%8 - dest%8) <= 2 && (diff == 6 || diff == 10 || diff == 15 || diff == 17) && !this->isSameColor(dest);
}

/*************************************************************************************/
/*                              BISHOP CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
bool Bishop::isPossibleMove(int dest)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return (diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest) && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*************************************************************************************/
/*                              ROOK CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
bool Rook::isPossibleMove(int dest)
{
    int src = this->getPieceSquare();
    return (sameRow(src, dest) || sameCol(src, dest)) && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*************************************************************************************/
/*                              QUEEN CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
bool Queen::isPossibleMove(int dest)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return ( ((diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest)) || (sameRow(src, dest) || sameCol(src, dest)) )
              && this->isPathFree(dest) && !this->isSameColor(dest);
}

/*************************************************************************************/
/*                              KING CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
bool King::isPossibleMove(int dest)
{
    vector<Piece*> board = chess.getBoard();

    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return ( (diff == 1 || diff == 7 || diff == 8 || diff == 9) && !this->isSameColor(dest) ) || ( (diff == 3 || diff == 4) && this->canCastle(dest) && this->isSameColor(dest) );
}

bool King::canCastle(int dest)
{
    int src = this->getPieceSquare();
    int increment = src > dest ? -1 : 1;

    vector<Piece*> board = chess.getBoard();
    stack<Piece*> stack = chess.getCheckStack();

    if(this->getPieceMoveInfo() || board[dest]->getPieceMoveInfo() || chess.getCheck() || !board[dest]->isRook() || !sameRow(src, dest))
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
        // pawn can only attack sideways, but the board isn't updated yet so it will always be invalid move
        if( !elem->isEmpty() && !this->isSameColor(elem->getPieceSquare()) &&
            ( (!elem->isPawn() && elem->isPossibleMove(dest)) ||
              (elem->isPawn() && elem->isPieceWhite() && (elem->getPieceSquare() - dest == 9 || elem->getPieceSquare() - dest == 7)) ||
              (elem->isPawn() && elem->isPieceBlack() && (dest - elem->getPieceSquare() == 9 || dest - elem->getPieceSquare() == 7)) 
            ) )
        {
            return true;
        }
    }

    return false;
}

/*************************************************************************************/
/*                              LOCAL FUNCTIONS / OBJECTS                            */
/*************************************************************************************/
namespace
{
    // Description:     Determines if a destination square is in the pinning path
    //                  as sometimes you can simply move the pinned piece closer to the 
    //                  pinning piece.
    // Pre-condition:   'src'       - source square of pinned piece
    //                  'dest'      - destination square of pinned piece
    //                  'pin'       - source square of pinning piece    
    // Post-condition:  Allows the move (returns false) if piece can move closer to pinning
    //                  piece, else move is invalid (returns true)
    bool destNotInPinPath(int src, int dest, int pin)
    {
        int inc_dest = incrementChoice(src, dest);
        int inc_pin = incrementChoice(src, pin);

        return !(inc_dest != 0 && inc_pin != 0 && inc_dest == inc_pin && std::min(src, dest) + inc_pin <= std::max(src, dest));
    }

    // Description:     Determines if the source and destination squares are in the same column
    // Pre-condition:   'src'       - source square of piece
    //                  'dest'      - destination square of piece   
    // Post-condition:  Returns true if the source and destination are in the same column,
    //                  else retuns false.
    bool sameCol(int src, int dest)
    {
        return src % 8 == dest % 8;
    }

    // similar to sameCol(.), just for rows
    bool sameRow(int src, int dest)
    {
        return src / 8 == dest / 8;
    }

    // similar to sameCol(.), just for diagonals
    bool sameDiag(int src, int dest)
    {
        return std::abs(src/8 - dest/8) == std::abs(src%8 - dest%8); 
    }

    // Description:     Used to determine the coordinate of a pinned piece.
    // Pre-condition:   'chess'     - object is created
    //                  'src'       - source square of pinning piece
    //                  'dest'      - destination square of pinning piece
    // Post-condition:  If only 1 piece is in the path from 'src' to 'dest', return its coordinates
    //                  else, return -1 to indicate that there is either no piece or multiple
    //                  pieces in the path.
    int squareOfPieceInPath(int src, int dest)
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

    // Description:     Used to determine the increment to use when moving a piece from 'src' to 'dest'.
    // Pre-condition:   'src'       - source square of piece
    //                  'dest'      - destination square of piece
    // Post-condition:  Returns either 1 if moving in same row, 7 if moving in diagonal to the right,
    //                  8 if moving in same column, 9 if moving in diagonal to the left. If move does 
    //                  not correspond to one of the pieces, returns 0.
    int incrementChoice(int src, int dest)
    {
        if(sameRow(src, dest)) // row path
        {
            return 1;
        }
        else if(sameCol(src, dest)) // column path
        {
            return 8;
        }
        else if(sameDiag(src, dest)) // diagonal path
        {
            return std::abs(src - dest) % 7 == 0 ? 7 : 9;
        }
        else
        {
            return 0;
        }
    }

    // Description:     Used to determine the coordinate of a pinned piece.
    // Pre-condition:   'chess'     - object is created
    //                  'src'       - source square of pinning piece
    //                  'board'     - initialize board representing the current position
    //                  'enemy'     - true if king color differs from piece color, else false
    // Post-condition:  Returns the coordinate position of the king, based on the current board
    //                  representation and color determined by 'enemy'. If king is not found
    //                  returns -1.
    int findKingPos(int src, const vector<Piece*> & board, bool enemy)
    {
        for(auto elem : board)
        {
            if(enemy && elem->isKing() && !elem->isSameColor(src))
                return elem->getPieceSquare();
            else if(!enemy && elem->isKing() && elem->isSameColor(src))
                return elem->getPieceSquare();
        } 

        return -1; // king was not found
    }
} // unnamed namespace

/*************************************************************************************/
/*                              GLOBAL FUNCTIONS / OBJECTS                           */
/*************************************************************************************/
namespace chessCAMO
{
    // Description:     Places the pieces on the board at their correct starting positions
    // Pre-condition:   'chess'      - global object is initialized
    //                  'board_size'  - 8x8 board has 64 index positions
    // Post-condition:  Instantiates new objects corresponding to the pieces, places them
    //                  in the corresponding index of the board vector and set the global
    //                  object's board variable 
    void boardInit(int board_size)
    {
        vector<Piece*> board = chess.getBoard();

        // initialize the board
        for(int i = 0; i < board_size; i++)
        {
            /******************* BLACK *******************/
            if(i < board_size/4)            // first 2 rows
            {
                if(i == 0 || i == 7)        // rook
                {
                    board.push_back(new Rook(i, ROOK, BLACK));
                }
                else if(i == 1 || i == 6)   // knight
                {
                    board.push_back(new Knight(i, KNIGHT, BLACK)); 
                }
                else if(i == 2 || i == 5)   // bishop
                {
                    board.push_back(new Bishop(i, BISHOP, BLACK)); 
                }
                else if(i == 3)             // queen
                {
                    board.push_back(new Queen(i, QUEEN, BLACK));
                }
                else if(i == 4)             // king
                {
                    board.push_back(new King(i, KING, BLACK));
                }
                else                        // pawn
                {
                    board.push_back(new Pawn(i, PAWN, BLACK));
                }
            }

            /********** NEUTRAL (EMPTY SQUARES) **********/
            else if(board_size/4 <= i && i < board_size*3/4) 
            {   
                board.push_back(new Empty(i, EMPTY, NEUTRAL)); // middle 4 rows
            }

            /***************** WHITE *******************/
            else                                // last 2 rows
            {
                if(i == 56 || i == 63)          // rook
                {
                    board.push_back(new Rook(i, ROOK, WHITE));
                }
                else if(i == 57 || i == 62)     // knight
                {
                    board.push_back(new Knight(i, KNIGHT, WHITE)); 
                }
                else if(i == 58 || i == 61)     // bishop
                {
                    board.push_back(new Bishop(i, BISHOP, WHITE));
                }
                else if(i == 59)                // queen
                {
                    board.push_back(new Queen(i, QUEEN, WHITE));
                }
                else if(i == 60)                // king
                {
                    board.push_back(new King(i, KING, WHITE));
                }
                else                            // pawn
                {
                    board.push_back(new Pawn(i, PAWN, WHITE));
                }
            }
        }

        // setting the global object's new board representation
        chess.setBoard(board);

        // printing the board and letting user know whose turn it is
        // white always starts first in chess!
        chessCAMO::printBoard(board);
        cout << "___________________________________________________" << endl;
        chessCAMO::printMessage("\n            White's move\n", CYAN);
    }

    // Description:     Iterates through the pieces on a current board representation to 
    //                  produce the board on the console screen
    // Pre-condition:   'board'      - a board is created and contains pieces
    // Post-condition:  Each piece of the current board representation is printed to the
    //                  screen using a corresponding letter inside a formatted board
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

    // Description:     At any moment, the players can either continue, draw, or resign
    // Pre-condition:   None
    // Post-condition:  Depending on the users choice, the program either continues
    //                  ('y' || 'd' + 'n') or terminates ('d' + 'y' || 'r')
    void drawOrResign()
    {
        char user_input, draw_reply;
        string message;

        chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw] ", PINK);
        cin >> user_input;
        cin.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)

        // error check
        while((int) user_input != 89 && (int) user_input != 121 && (int) user_input != 114 && (int) user_input != 82 && (int) user_input != 68 && (int) user_input != 100)
        {
            chessCAMO::printMessage("Pick one of the choices... try again!", YELLOW);
            chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw] ", PINK);

            cin >> user_input; // get new input
            cin.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)
        }

        if((int) user_input == 114 || (int) user_input == 82) // r
        {
            message = chess.getTurn() == WHITE ? "\nWhite resigned -> Black wins\n" : "\nBlack resigned -> White wins\n";
            chessCAMO::printMessage(message, CYAN);
            exit(0);
        }
        else if((int) user_input == 68 || (int) user_input == 100) // d
        {
            chessCAMO::printMessage("\nOffered draw... do you accept? [y -> yes, n -> no] ", PINK);
            cin >> draw_reply;
            cin.ignore(100, '\n'); // ignore rest of the previous input

            // error check
            while((int) draw_reply != 89 && (int) draw_reply != 121 && (int) draw_reply != 78 && (int) draw_reply != 110)
            {
                chessCAMO::printMessage("Pick one of the choices... try again! ", YELLOW);
                chessCAMO::printMessage("\nOffered draw... do you accept? [y -> yes, n -> no] ", PINK);
                cin >> draw_reply; // get new input
                cin.ignore(100, '\n'); // ignore rest of the previous input
            }

            if((int) draw_reply == 89 || (int) draw_reply == 121) // y
            {
                chessCAMO::printMessage("\nGame drawn by agreement\n", CYAN);
                exit(0);
            }
            else if((int) draw_reply == 78 || (int) draw_reply == 110) // n
            {
                chessCAMO::printMessage("\nDraw rejected. Game continues...\n", CYAN);
                return;
            }
        }
        else // y
        {
            return ; // do nothing
        }
    }

    // Description:     Prints the given message ('text') with a given 'color' to console
    // Pre-condition:   'text'      - message is created
    //                  'color'     - one of the defined values at the top of the file
    // Post-condition:  The message is printed to the screen with color chosen and then
    //                  the color is changed back to default prior to return
    void printMessage(string text, int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        cout << text;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
    }
}