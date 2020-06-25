/**
 * \page chess Chess Implementation File (Project Description)
 * 
 * <b>Title</b><br>
 * <span>&emsp;&emsp;&emsp;chess.cpp </span>
 * \author Lior Bragilevsky<br>
 * 
 * <b>Related</b><br>
 * <span>&emsp;&emsp;&emsp;chess.h</span><br> 
 * 
 * <b>Project</b><br>
 * <span>&emsp;&emsp;&emsp;chessCAMO</span><br>
 * \version \version_num
 * \date \today
 * 
 * <i>chessCAMO</i> is a newly proposed chess variant that stands for <b>C</b>alculations <b>A</b>lways <b>M</b>ake <b>O</b>pportunities
 * and was inspired by the very popular chess variant <a href="https://en.wikipedia.org/wiki/Crazyhouse" target="__blank">Crazyhouse</a>.
 *
 * The "CAMO" portion comes from the idea that in this variant, at any turn, a player can sacrifice making a move to replace one of their
 * pieces with a piece from the "piece reservoir". This has the affect of camouflaging each piece that is currently on the board. 
 * 
 * <b>Variant's Rules</b>
 * 1. The piece reservoir is limited in size and cannot be re-stocked with pieces.
 * 2. Once a player decides to use a piece from the reservoir to replace one of his existing pieces on the board, their turn is over.
 * 3. An obvious rule is that player cannot use the reservoir while in check as they must make a move to avoid check.
 * 4. Additionally, just like in the <i>Crazyhouse</i> variant, a player cannot directly promote a pawn from the reservoir.
 *    That is a pawn from the reservoir must be placed on the board first and is only allowed to be promoted on the next move.
 */

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

/*************************************************************************************/
/*                              LOCAL FUNCTIONS / OBJECTS                            */
/*************************************************************************************/
/**
 * @brief   This anonymous namespace contains the local functions related to chessCAMO which 
 *          are mainly used as helper functions to determine critical information regarding
 *          a given move choice.
 */
namespace
{
    /**
     * @brief      Determines if a destination square is in the pinning path
     *             as sometimes you can simply move the pinned piece closer to the 
     *             pinning piece.
     *
     * @param[in]  src   The source square of pinned piece
     * @param[in]  dest  The destination square of pinned piece
     * @param[in]  pin   Source square of pinning piece
     *
     * @return     Allows the move (returns false) if piece can move closer to pinning
     *             piece, else move is invalid (returns true)
     */
    bool destNotInPinPath(int src, int dest, int pin);

    /**
     * @brief      Determines if the source and destination squares are in the same column
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same column,
     *             else False.
     */
    bool sameCol(int src, int dest);

    /**
     * @brief      Determines if the source and destination squares are in the same row
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same row,
     *             else False.
     */
    bool sameRow(int src, int dest);

    /**
     * @brief      Determines if the source and destination squares are in the same diagonal
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same diagonal,
     *             else False.
     */
    bool sameDiag(int src, int dest);

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param[in]  dest   The destination square of pinning piece
     * @param      chess  The chess object is created
     *
     * @return     If only 1 piece is in the path from 'src' to 'dest', return its coordinates
     *             else, return -1 to indicate that there is either no piece or multiple
     *             pieces in the path.
     */
    int squareOfPieceInPath(int src, int dest, Chess * chess);

    /**
     * @brief      Used to determine the increment to use when moving a piece from 'src' to 'dest'.
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     Returns either 1 if moving in same row, 7 if moving in diagonal to the right,
     *             8 if moving in same column, 9 if moving in diagonal to the left. If move does 
     *             not correspond to one of the pieces, returns 0.
     */
    int incrementChoice(int src, int dest);

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param      chess  The chess object is created
     * @param[in]  enemy  True if king color differs from piece color, else false
     *
     * @return     Returns the coordinate position of the king, based on the current board
     *             representation and color determined by 'enemy'.
     */
    int findKingPos(int src, Chess * chess, bool enemy); 
} // unnamed namespace (makes these functions local to this implementation file)

/*************************************************************************************/
/*                              CHESS CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      Destructor - destroys the objects.
 */
Chess::~Chess()
{
    vector<Piece*> board = this->getBoard();
    while(!board.empty())
    {
        board.pop_back();
    }
}

/**
 * @brief      Places the pieces on the board at their correct starting positions
 *
 * \pre
 * The chess object is intialized
 * 
 * \post 
 * Instantiates new objects corresponding to the pieces, places them
 * in the corresponding index of the board vector and set the global
 * object's board variable
 */  
void Chess::boardInit()
{
    vector<Piece*> board(64);

    // initialize the board
    for(unsigned int i = 0; i < board.size(); i++)
    {
        /******************* BLACK *******************/
        if(i < board.size()/4)          // first 2 rows
        {
            if(i == 0 || i == 7)        // rook
            {
                board[i] = new Rook(i, ROOK, BLACK);
            }
            else if(i == 1 || i == 6)   // knight
            {
                board[i] = new Knight(i, KNIGHT, BLACK); 
            }
            else if(i == 2 || i == 5)   // bishop
            {
                board[i] = new Bishop(i, BISHOP, BLACK); 
            }
            else if(i == 3)             // queen
            {
                board[i] = new Queen(i, QUEEN, BLACK);
            }
            else if(i == 4)             // king
            {
                board[i] = new King(i, KING, BLACK);
            }
            else                        // pawn
            {
                board[i] = new Pawn(i, PAWN, BLACK);
            }
        }

        /********** NEUTRAL (EMPTY SQUARES) **********/
        else if(i < board.size()*3/4) 
        {   
            board[i] = new Empty(i, EMPTY, NEUTRAL); // middle 4 rows
        }

        /***************** WHITE *******************/
        else                                // last 2 rows
        {
            if(i == 56 || i == 63)          // rook
            {
                board[i] = new Rook(i, ROOK, WHITE);
            }
            else if(i == 57 || i == 62)     // knight
            {
                board[i] = new Knight(i, KNIGHT, WHITE); 
            }
            else if(i == 58 || i == 61)     // bishop
            {
                board[i] = new Bishop(i, BISHOP, WHITE);
            }
            else if(i == 59)                // queen
            {
                board[i] = new Queen(i, QUEEN, WHITE);
            }
            else if(i == 60)                // king
            {
                board[i] = new King(i, KING, WHITE);
            }
            else                            // pawn
            {
                board[i] = new Pawn(i, PAWN, WHITE);
            }
        }
    }

    // setting the global object's new board representation
    this->setBoard(board);

    // printing the board and letting user know whose turn it is
    // white always starts first in chess!
    chessCAMO::printBoard(board);
    cout << "___________________________________________________" << endl;
    chessCAMO::printMessage("\n            White's move\n", CYAN);
}

/**
 * @brief      Stores relevant board representation information that is useful when "undoing" a move is required
 *
 * @param      board            The current board representation
 * @param      squares_prior    The previous board representation's element square information
 * @param      moved_prior      The previous board representation's element move information
 * @param      enpassant_prior  The previous board representation's element en-passant ability information
 * @param[in]  type             Either "store" or "restore", corresponding to storing information prior to a move
 *                              being made or restoring information after the move was made and an undo was applied.
 * 
 * \pre 
 * None
 * 
 * \post
 * Updates the board and relevant information flags (square, has moved, en-passant ability).
*/
void Chess::storeOrRestore(vector<Piece*> & board, vector<int> & squares_prior, vector<bool> & moved_prior, vector<pair<bool, bool>> & enpassant_prior, string type)
{
    for(auto & elem : board)
    {
        int index = &elem - &board[0];
        if(type == "store")
        {
            squares_prior[index] = elem->getPieceSquare();
            moved_prior[index] = elem->getPieceMoveInfo();
            enpassant_prior[index].first = elem->getEnPassantLeft();
            enpassant_prior[index].second = elem->getEnPassantRight();
        }
        else // type == "restore"
        {
            elem->setPieceSquare(squares_prior[index]);
            elem->setPieceMoveInfo(moved_prior[index]);
            elem->setEnPassantLeft(enpassant_prior[index].first);
            elem->setEnPassantRight(enpassant_prior[index].second);
        }
    }
}

/**
 * @brief      Moves a piece on the board from 'src' to 'dest' if conditions
 *             for a legal move are met.
 *
 * @param[in]  src   The source square (piece's current location)
 * @param[in]  dest  The destination square (piece's ending location)
 * @param      in    input stream type (stdin or file)
 * 
 * \pre
 * The chess object is created.
 * 
 * \post
 * The pieces at 'src' and 'dest' positions are swapped.
 * If needed (attacking, castling, etc.) an empty square is made.
 * The board's state is updated to indicate that the move occured.
 * On failure, an error message is printed and user is asked to retry.
 */
void Chess::makeMove(int src, int dest, istream &in)
{
    vector<Piece*> board = this->getBoard();
    stack<Piece*> check_stack = this->getCheckStack();
    Piece *king, *piece;

    vector<int> squares_prior(64);
    vector<bool> moved_prior(64);
    vector<pair<bool, bool>> enpassant_prior(64);
    
    if(0 <= src && src <= 63 && board[src]->isLegalMove(dest, this) && board[src]->getPieceColor() == this->getTurn())
    {   
        // pawn promotion
        if(dest/8 == 0 || dest/8 == 7)
        {
            board[src]->promotePawn(dest, this, in);
        }

        // store relevant piece information
        this->storeOrRestore(board, squares_prior, moved_prior, enpassant_prior, "store");

        // make the appropriate move from 'src' to 'dest'
        this->makeMoveForType(src, dest);

        // when in double check you must move the king
        if(this->getDoubleCheck())
        {      
            king = check_stack.top();
            check_stack.pop();

            for(auto & elem : board)
            {    
                // if the move failed, undo the board representation and do not set check_stack               
                if(this->needUndoMove(king, elem, "double"))
                {
                    this->undoMove(board, squares_prior, moved_prior, enpassant_prior);
                    return;
                }
            }

            // if here, did not return so set the appropriate member variables
            this->setCheckStack(check_stack);
            this->setDoubleCheck(false);
        }

        // when in single check you have three choices: move the king, defend the path, attack the checking piece
        else if(this->getCheck())
        {
            piece = check_stack.top();
            check_stack.pop();
            king = check_stack.top();
            check_stack.pop();

            // if the move failed, undo the board representation and do not set check_stack               
            if(this->needUndoMove(king, piece, "single"))
            {
                this->undoMove(board, squares_prior, moved_prior, enpassant_prior); 
                return;
            }

            // if here, did not return so set the appropriate member variables
            this->setCheckStack(check_stack);
            this->setCheck(false);
        }

        board = this->getBoard();
        board[dest]->enPassantHandling(src, this); // en-passant checking/updating

        // did the move cause a check?
        if(board[src]->causeCheck(dest, this) && !board[src]->causeDoubleCheck(dest, this)) 
        {
            this->isCheckmate("single"); // check for checkmates
        }

        // did the move cause a double check?
        if(board[src]->causeDoubleCheck(dest, this)) 
        {
            this->isCheckmate("double"); // check for checkmates
        }

        // check for stalemate
        if(this->isStalemate()) 
        {
            this->handleStalemate();
        }

        // after a move was made and all the above checks passed, can finally change the turn
        this->handleChangeTurn();
    }
    else
    {
        chessCAMO::printBoard(board);
        chessCAMO::printMessage("\nInvalid move! Try again...\n", YELLOW);
    }
}

/**
 * @brief      Undo a move if needed and restore the piece information on the new board representation
 *
 * @param      board            The current board representation
 * @param      squares_prior    The previous board representation's element squares
 * @param      moved_prior      The previous board representation's element move information
 * @param      enpassant_prior  The previous board representation's element en-passant ability information
 * 
 * \pre
 * Illegal move is made on the current board representation.
 * 
 * \post
 * The stack of board positions is decremented by one position which represented the illegal move
 */
void Chess::undoMove(vector<Piece*> & board, vector<int> & squares_prior, vector<bool> & moved_prior, vector<pair<bool, bool>> & enpassant_prior)
{
    this->setBoardPositions().pop();
    board = this->getBoard();
    this->storeOrRestore(board, squares_prior, moved_prior, enpassant_prior, "restore");
}

/**
 * @brief      Decide if a move caused a checkmate according to 'check_type'
 *
 * @param[in]  check_type  Either "single" or "double" corresponding to the check on the board
 * 
 * \pre
 * The chess object is created.
 * 
 * \post
 * If board's state is in checkmate, calls handleCheckmate() to print messages
 * to console indicating the winner. Else, game continues as usual.
 */
void Chess::isCheckmate(string check_type)
{
    stack<Piece*> check_stack = this->getCheckStack();
    Piece *king, *piece;

    if(check_type == "double") // this type of check requires the king to move
    {
        king = check_stack.top();

        // checkmate due to a double check
        if(doubleCheckPieceIterator(king))
            handleCheckmate();
        else // was not checkmate so can state that it is double check
            chessCAMO::printMessage("\n            Double Check!\n\n", CYAN); 
    }   

    else // check_type == "single"
    {
        // will not be set, so next time this will be identical
        piece = check_stack.top();
        check_stack.pop();
        king = check_stack.top();

        // checkmate due to a single piece
        if(singleCheckPieceIterator(piece, king))
            handleCheckmate();
        else // was not checkmate so can state that it is check
            chessCAMO::printMessage("\n                Check!\n\n", CYAN); 
    }
}

/**
 * @brief      Decide if a move caused a stalemate
 * \pre
 * The chess object is created.
 * 
 * \post
 * Calls Chess::handleStalemate() to print messages to console indicating that game is drawn, if needed.
 * 
 * @return     True if board's state is in stalemate, else False and game continues as usual.
 */
bool Chess::isStalemate()
{
    vector<Piece*> board = this->getBoard();

    // this is a list of all the possible increments (moves) a piece on the board could have
    int possible_moves[22] = {1, 6, 7, 8, 9, 10, 14, 15, 16, 17, 18,
                              21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63};
    
    for(const auto & elem : board)
    {
        if(elem->getPieceColor() == this->switchTurn())
        {
            for(int move : possible_moves)
            {
                if( elem->isLegalMove(elem->getPieceSquare() + move, this) ||
                    elem->isLegalMove(elem->getPieceSquare() - move, this) )
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
/**
 * @brief      A move can be one of: attack, castle, en-passant, regular
 *
 * @param[in]  src   The source square of piece
 * @param[in]  dest  The destination square of piece 
 * 
 * \pre
 * The chess object is created 
 * 
 * \post
 * Swaps the pieces on the board according to 'src' and 'dest' and proper
 * chess rules, using pieceSwap(.). If a new empty square must be created,
 * this is handled. Returns board representation with the made move.
 */
void Chess::makeMoveForType(int src, int dest)
{
    vector<Piece*> board = this->getBoard();

    // castling move
    if(board[src]->canCastle(dest, this))
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
    else if((board[src]->getEnPassantLeft() || board[src]->getEnPassantRight()) && std::abs(src-dest) != 8)
    {
        int sign = board[src]->isPieceWhite() ? 1 : -1;

        // delete the pawn that caused en-passant (make it an empty square)
        if(std::abs(src-dest) == 7)
        {
            board[src+sign] = new Empty(src+sign, EMPTY, NEUTRAL);
        }
        else // std::abs(src-dest) == 9
        {
            board[src-sign] = new Empty(src-sign, EMPTY, NEUTRAL);
        }

        // after the violating pawn is removed, can make the move
        this->pieceSwap(src, dest, board);
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

/**
 * @brief      Used in makeMoveForType(.) to swap pieces on the board
 *
 * @param[in]  src   The source square of piece
 * @param[in]  dest  The destination square of piece 
 * @param      board  The current board representation
 * 
 * \pre 
 * The chess object is created
 * 
 * \post
 * Swaps the pieces on the board according to 'src' and 'dest'.
 */
void Chess::pieceSwap(int src, int dest, vector<Piece*> & board)
{
    board[src]->setPieceSquare(dest);
    board[dest]->setPieceSquare(src);
    std::swap(board[src], board[dest]);
}

/**
 * @brief      Indicates who will move next via a message to console
 * 
 * \pre 
 * The chess object is created
 * 
 * \post
 * Player turn is switched, board is printed, and message is displayed
 * if game is not over to indicate whose turn it is.
 */
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

/**
 * @brief      Indicates which player won by checkmate via a message to console
 * 
 * \pre 
 * The chess object is created. A move was made (cannot checkmate in less than 2 moves in theory).
 * 
 * \post
 * Object's checkmate state is set to true (to end the algorithm)
 */
void Chess::handleCheckmate()
{
    string message = this->getTurn() == WHITE ? "\n      White won by Checkmate!\n\n" : "\n      Black won by Checkmate!\n\n";
    chessCAMO::printMessage(message, CYAN);
    this->setCheckmate(true);
}

/**
 * @brief      Indicates the game is drawn via a message to console
 * \pre 
 * The chess object is created. A move was made.
 * 
 * \post
 * Object's stalemate state is set to true (to end the algorithm)
 */
void Chess::handleStalemate()
{
    string message = this->switchTurn() == WHITE ? "\nWhite has no moves -> Game is Drawn!\n\n" : "\nBlack has no moves -> Game is Drawn!\n\n";
    chessCAMO::printMessage(message, CYAN);
    this->setStalemate(true);
}

/**
 * @brief      After a move is made, can undo it if move was invalid and return to previous board state
 * 
 * @param      king             The king that is being attacked currently
 * @param      piece            The piece that is attacking the king currently
 * @param[in]  check_type       The check type (single or double)
 * 
 * \pre 
 * The chess object is created. A move was made.
 * 
 * \post
 * None
 *
 * @return     True if after move, the 'king' is still in check (single or double) or the move was
 *             invalid, output warning message and undo the move. Else, False and continue the game
 *             without undoing the move.
 */
bool Chess::needUndoMove(Piece* king, Piece* piece, string check_type)
{
    vector<Piece*> board = this->getBoard();

    if(piece->isPossibleMove(king->getPieceSquare(), this))
    {   
        // invalid move checking when a player is in check (single/double)
        if(check_type == "double")
        {
            chessCAMO::printBoard(board);
            chessCAMO::printMessage("You are in double check! Try again...\n", YELLOW);
        }
        else // check_type == "single"
        {
            chessCAMO::printBoard(board);
            chessCAMO::printMessage("You are in check! Try again...\n", YELLOW);
        }
        
        return true; 
    }
    else
    {
        return false; // did not undo the move
    }
} 
       
/**
 * @brief      If in a single check, see if piece can defend the king, capture attacking piece,
 *             or move the king out of check. Used in isCheckmate("single")
 *
 * @param      piece  The piece that is attacking the king currently
 * @param      king   The king that is being attacked currently
 *
 * \pre 
 * The chess object is created. A move was made.
 * 
 * \post
 * None
 *
 * @return     True if no legal moves found (checkmate), else False and make the move
 */
bool Chess::singleCheckPieceIterator(Piece* piece, Piece* king)
{
    vector<Piece*> board = this->getBoard();
    int increment, src = piece->getPieceSquare(), dest = king->getPieceSquare();

    // can a piece defend the king from check?
    if(!piece->isKnight())
    {
        increment = src > dest ? -1*incrementChoice(src, dest) : incrementChoice(src, dest);
        for(const auto & elem : board)
        {
            if(elem->isSameColor(dest, this) && !elem->isKing())
            {
                for(int pos = src; pos != dest; pos += increment)
                {   
                    if(elem->isPossibleMove(pos, this))
                    {
                        return false;
                    }
                }
            }
        }
    }
    else // for a Knight check, to defend the king a piece must take the knight
    {
        for(const auto & elem : board)
        {
            if(!elem->isKing() && elem->isSameColor(dest, this) && elem->isPossibleMove(src, this))
            {
                return false;
            }
        }
    }

    // can king move out of check?
    return this->doubleCheckPieceIterator(king);
}

/**
 * @brief      If in a double check, see if the king can move out of check as this is the only
 *             valid move option. Used in isCheckmate("double").
 *
 * @param      king  The king that is being attacked currently
 *
 * \pre 
 * The chess object is created.
 * 
 * \post
 * None
 *
 * @return     True if no legal moves found (checkmate), else False and make the move
 */
bool Chess::doubleCheckPieceIterator(Piece* king)
{
    int src = king->getPieceSquare();
    int king_moves[4] = {1, 7, 8, 9}; // possible king moves (absolute values here)

    // can king move out of check?
    for(auto move : king_moves)
    {
        if(king->isLegalMove(src + move, this) || king->isLegalMove(src - move, this))
        {
            return false;
        }
    }
        
    return true; // no legal moves found ? true : false
}

/**
 * @brief      Decides whose turn it is currently and updates the private member variable ('turn') accordingly
 *
 * \pre 
 * The chess object is created.
 * 
 * \post
 * None
 *
 * @return     'chess.turn' is set to the correct player
 */
pieceColor Chess::switchTurn()
{
    return this->getTurn() == WHITE ? BLACK : WHITE;
}

/*************************************************************************************/
/*                              PIECE CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      Determines if 2 pieces have the same color
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if source piece color matches destination piece color, False otherwise.
 */
bool Piece::isSameColor(int dest, Chess *chess)
{

    vector<Piece*> board = chess->getBoard();

    // cannot use this->getPieceColor() here since the board might be updated
    return board[this->getPieceSquare()]->getPieceColor() == board[dest]->getPieceColor();
}

/**
 * @brief      Determines if a given piece is pinned to the king by opposing piece
 *
 * @param[in]  dest   The destination square of the piece  
 * @param      chess  The chess object
 *
 * @return     True if piece is pinned to the king and moving to 'dest' will cause
 *             the path (pinning piece -> king from pinned piece side) to be free, False otherwise.
 */
bool Piece::isPinned(int dest, Chess *chess)
{
    int king_pos, src = this->getPieceSquare();
    vector<Piece*> board = chess->getBoard();

    if(!this->isKing())
    {
        king_pos = findKingPos(src, chess, false); // same color king position

        for(const auto & elem : board)
        {
            if( !elem->isSameColor(king_pos, chess) &&
                (elem->isBishop() || elem->isRook() || elem->isQueen()) &&
                squareOfPieceInPath(elem->getPieceSquare(), king_pos, chess) == src )
            {   
                return destNotInPinPath(src, dest, elem->getPieceSquare());
            }
        }
    }

    return false; // king cannot be pinned to itself
}

/**
 * @brief      Determines if the path from the piece to its destination is empty
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if squares along the path (src, dest) are empty, False otherwise.
 */
bool Piece::isPathFree(int dest, Chess *chess)
{
    int increment, src = this->getPieceSquare();
    vector<Piece*> board = chess->getBoard();

    // knight can skip over pieces so it's path is always free
    if(this->isKnight())
    {
        return true;
    }
    else
    {
        // get the increment value for the path and see if the squares in (src, dest) are empty
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

/**
 * @brief      Determines if a move is legal based on the rules of chess
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if moving the piece to 'dest' is legal from any type 
 *             of move and piece, False otherwise.
 * 
 * \note       A possible move, is not necessarily legal.
 */
bool Piece::isLegalMove(int dest, Chess *chess)
{
    return 0 <= dest && dest <= 63 && this->getPieceSquare() != dest && 
           this->isPossibleMove(dest, chess) && !this->isPinned(dest, chess) && !this->movedIntoCheck(dest, chess);
}

/**
 * @brief      Did the move cause a check?
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return    True if moving the piece to 'dest' now threatens the opposing king.
 *            False otherwise.
 */
bool Piece::causeCheck(int dest, Chess *chess)
{
    stack<Piece*> check_stack = chess->getCheckStack();
    vector<Piece*> board = chess->getBoard();
    
    // was already in check before the move
    if(!chess->getCheckStack().empty())
    {
        return false;
    }
    else
    {
        int king_pos = findKingPos(dest, chess, true); // opposite color king position

        if(board[dest]->isPathFree(king_pos, chess) && board[dest]->isPossibleMove(king_pos, chess))
        {
            // push the king and checking piece onto the stack and set the corresponding 
            // object variables (checkStack and setCheck)
            check_stack.push(board[king_pos]);
            check_stack.push(board[dest]);
            chess->setCheckStack(check_stack);
            chess->setCheck(true);
        }
    } 

    return chess->getCheck();
}

/**
 * @brief      Did the move cause a double check?
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if moving the piece to 'dest' now threatens the opposing king,
 *             and an additional piece from the same side also has a legal move towards the 
 *             opposing king.
 *             False otherwise.
 */
bool Piece::causeDoubleCheck(int dest, Chess *chess)
{
    int king_pos, checking_piece_counter = 0;
    vector<Piece*> board = chess->getBoard();
    stack<Piece*> check_stack = chess->getCheckStack();

    king_pos = findKingPos(dest, chess, true); // opposite color king position
    check_stack.push(board[king_pos]); // make the king last in the stack

    // how many pieces are checking the king
    for(const auto & elem : board)
    {
        if(elem->isLegalMove(king_pos, chess))
        {
            checking_piece_counter++;
        }
    }

    // double check if 2 pieces are attacking the king
    if(checking_piece_counter == 2)
    {
        chess->setCheckStack(check_stack);
        chess->setDoubleCheck(true);
        return true;
    }

    return chess->getDoubleCheck();
}

/*************************************************************************************/
/*                              PAWN CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
  * \note
  * Can move 1 or 2 square (if not moved yet) forwards, attack diagonally 1 square,
  * en-passant, and promote.
  *   
  * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool Pawn::isPossibleMove(int dest, Chess *chess)
{
    vector<Piece*> board = chess->getBoard();

    bool legal = false;
    int src = this->getPieceSquare();
    int diff = this->isPieceWhite() ? src-dest : dest - src;
    int sign = this->isPieceWhite() ? 1 : -1;

    // on attack it can move diagonally, first move can be 2 squares forward,
    // en-passant is possible for one move if conditions are met
    if(this->getPieceMoveInfo())
    {
        legal = (diff == 8 && board[dest]->isEmpty()) || 
                ( ( diff == 7 && ( !board[dest]->isEmpty() || (board[src]->getEnPassantRight() && board[src+sign]->isPawn()) ) ) ||
                  ( diff == 9 && ( !board[dest]->isEmpty() || (board[src]->getEnPassantLeft() && board[src-sign]->isPawn()) ) ) );
    }
    else // cannot en-passant if you have not moved yet
    {
        legal = ((diff == 8 || diff == 16) && board[dest]->isEmpty()) ||
                ((diff == 7 || diff == 9) && !board[dest]->isEmpty());
    }

    return legal && this->isPathFree(dest, chess) && !this->isSameColor(dest, chess);
}

/**
    * \see virtual Piece::enPassantHandling(int dest, Chess *chess)
 */
void Pawn::enPassantHandling(int src, Chess *chess)
{
    vector<Piece*> board = chess->getBoard();
    int dest = this->getPieceSquare();
    int sign = this->isPieceWhite() ? 1 : -1;

    // First, cancel en-passant abilities of all pawns.
    // Then determine which pawn can have en-passant abilities
    for(const auto & elem : board) 
    {
        elem->setEnPassantLeft(false);
        elem->setEnPassantRight(false);
    }

    // pawn moves 2 squares ...
    if(std::abs(src-dest) == 16)
    {
        // and there is a pawn to its left
        if(board[dest-sign]->isPawn() && !board[dest-sign]->isSameColor(dest, chess))
            board[dest-sign]->setEnPassantLeft(true);

        // and there is a pawn to its right
        if(board[dest+sign]->isPawn() && !board[dest+sign]->isSameColor(dest, chess))
            board[dest+sign]->setEnPassantRight(true);
    }
    
}

/**
    * \see virtual Piece::promotePawn(int dest, Chess *chess, istream &in)
 */
void Pawn::promotePawn(int dest, Chess *chess, istream &in)
{
    vector<Piece*> board = chess->getBoard();
    bool white_turn = chess->getTurn() == WHITE;

    char piece;
    int src = this->getPieceSquare();
    while(true)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), YELLOW);
        cout << "\nWhich Piece: Q/q | R/r | B/b | N/n? " << endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
        in >> piece;
        if(piece == 'Q' || piece == 'q')
        {
            board[src] = white_turn ? new Queen(dest, QUEEN, WHITE) : new Queen(dest, QUEEN, BLACK);
            break;
        }
        if(piece == 'R' || piece == 'r')
        {
            board[src] = white_turn ? new Rook(dest, ROOK, WHITE) : new Rook(dest, ROOK, BLACK);
            break;
        }
        if(piece == 'B' || piece == 'b')
        {
            board[src] = white_turn ? new Bishop(dest, BISHOP, WHITE) : new Bishop(dest, BISHOP, BLACK);
            break;
        }
        if(piece == 'N' || piece == 'n')
        {
            board[src] = white_turn ? new Knight(dest, KNIGHT, WHITE) : new Knight(dest, KNIGHT, BLACK);
            break;
        }

        chessCAMO::printMessage("Please make sure to pick correct value!\n", YELLOW);
    }

    chess->setBoard(board);
}

/*************************************************************************************/
/*                              KNIGHT CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
/**
  * \note
  * Can move (2 up/down or 2 left/right) and (1 left/right or 1 up/down), can jump over pieces.
  * 
  * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool Knight::isPossibleMove(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return std::abs(src/8 - dest/8) <= 2 && std::abs(src%8 - dest%8) <= 2 &&
           (diff == 6 || diff == 10 || diff == 15 || diff == 17) &&
           !this->isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              BISHOP CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/

/**
 * \note
 * Can move diagonally any number of squares
 * 
 * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool Bishop::isPossibleMove(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return (diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest) &&
           this->isPathFree(dest, chess) && !this->isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              ROOK CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * \note
 * Can move horizontally or vertically any number of squares.
 * 
 * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool Rook::isPossibleMove(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    return ( sameRow(src, dest) || sameCol(src, dest) ) &&
           this->isPathFree(dest, chess) && !this->isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              QUEEN CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * \note
 * Combines rook and bishop moves.
 * 
 * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool Queen::isPossibleMove(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    return ( ((diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest)) ||
             (sameRow(src, dest) || sameCol(src, dest)) ) &&
             this->isPathFree(dest, chess) && !this->isSameColor(dest, chess);
}

/*************************************************************************************/
/*                             KING CLASS - MEMBER FUNCTIONS                         */
/*************************************************************************************/
/**
 * \note
 * Combines rook and bishop moves but only 1 square
 * 
 * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
 */
bool King::isPossibleMove(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    int diff = std::abs(src - dest);
    int col_diff = std::abs(src%8 - dest%8), row_diff = std::abs(src/8 - dest/8);
    return ( (col_diff <= 1 && row_diff <= 1) && (diff == 1 || diff == 7 || diff == 8 || diff == 9) && !this->isSameColor(dest, chess) ) ||
           ( (diff == 3 || diff == 4) && this->canCastle(dest, chess) ) ;
}

 /**
 * \see virtual Piece::canCastle(int dest, Chess *chess)
 */
bool King::canCastle(int dest, Chess *chess)
{
    int src = this->getPieceSquare();
    int increment = src > dest ? -1 : 1;

    vector<Piece*> board = chess->getBoard();
    stack<Piece*> stack = chess->getCheckStack();

    if( this->getPieceMoveInfo() || board[dest]->getPieceMoveInfo() || chess->getCheck() || !board[dest]->isRook() )
    {
        return false;
    }
    else
    {
        for(const auto & elem : board)
        {
            if(!elem->isEmpty() && !elem->isSameColor(src, chess))
            {
                // king only moves 2 squares regardless of castle direction
                for(int square = src+increment; square != src+(2*increment); square += increment) 
                {   
                    if(elem->isPossibleMove(square, chess))
                    {
                        return false;
                    }
                }
            }
        }

        return this->isPathFree(dest, chess);
    }
}

/**
 * \see virtual Piece::movedIntoCheck(int dest, Chess *chess)
 */
bool King::movedIntoCheck(int dest, Chess *chess)
{
    vector<Piece*> board = chess->getBoard();
    int src, sign;

    pieceColor original_color = board[dest]->getPieceColor();
    bool ret_val = false;

    for(const auto & elem : board)
    {
        src = elem->getPieceSquare();
        sign = elem->isPieceWhite() ? 1 : -1;

        // change the color of the attacking piece so that if it is supported,
        // one can check if the supporting piece can move into that square (in which case
        // the king cannot capture the attacker)
        board[dest]->setPieceColor(NEUTRAL);

        // pawn can only attack sideways, but the board isn't updated yet so it will always be invalid move
        // checking dest-increment and increment since if a piece is only 1 square away dest-increment gives src=dest.
        // However, since move isn't made it is possible that a piece of same color causes isPossibleMove(dest, chess)
        // to be false, see 48-scholarMate.txt
        if( !elem->isEmpty() && !this->isSameColor(src, chess) && 
            ( ( !elem->isPawn() && elem->isPossibleMove(dest, chess) ) ||
              ( elem->isPawn() && (sign*(src - dest) == 9 || sign*(src - dest) == 7) ) ))
        {
            ret_val = true;
            break;
        }
    }

    board[dest]->setPieceColor(original_color);
    return ret_val;
}

/*************************************************************************************/
/*                              LOCAL FUNCTIONS / OBJECTS                            */
/*************************************************************************************/
namespace
{
    /**
     * @brief      Determines if a destination square is in the pinning path
     *             as sometimes you can simply move the pinned piece closer to the 
     *             pinning piece.
     *
     * @param[in]  src   The source square of pinned piece
     * @param[in]  dest  The destination square of pinned piece
     * @param[in]  pin   Source square of pinning piece
     *
     * @return     Allows the move (returns false) if piece can move closer to pinning
     *             piece, else move is invalid (returns true)
     */
    bool destNotInPinPath(int src, int dest, int pin)
    {
        int inc_dest = incrementChoice(src, dest);
        int inc_pin = incrementChoice(src, pin);

        return !(inc_dest != 0 && inc_dest == inc_pin);
    }

    /**
     * @brief      Determines if the source and destination squares are in the same column
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same column,
     *             else False.
     */
    bool sameCol(int src, int dest)
    {
        return src % 8 == dest % 8;
    }

    /**
     * @brief      Determines if the source and destination squares are in the same row
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same row,
     *             else False.
     */
    bool sameRow(int src, int dest)
    {
        return src / 8 == dest / 8;
    }

    /**
     * @brief      Determines if the source and destination squares are in the same diagonal
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece 
     *
     * @return     True if the source and destination are in the same diagonal,
     *             else False.
     */
    bool sameDiag(int src, int dest)
    {
        return std::abs(src/8 - dest/8) == std::abs(src%8 - dest%8); 
    }

        /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param[in]  dest   The destination square of pinning piece
     * @param      chess  The chess object is created
     *
     * @return     If only 1 piece is in the path from 'src' to 'dest', return its coordinates
     *             else, return -1 to indicate that there is either no piece or multiple
     *             pieces in the path.
     */
    int squareOfPieceInPath(int src, int dest, Chess *chess)
    {
        int increment;
        vector<Piece*> board = chess->getBoard();
        vector<int> pieces_in_path;

        // determine the increment along the path from 'src' to 'dest'
        // and store all non empty squares in 'pieces_in_path'
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

        // only 1 piece ? return its square : return -1 (equivalent to false)
        return pieces_in_path.size() == 1 ? pieces_in_path[0] : -1;
    }

    /**
     * @brief      Used to determine the increment to use when moving a piece from 'src' to 'dest'.
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     Returns either 1 if moving in same row, 7 if moving in diagonal to the right,
     *             8 if moving in same column, 9 if moving in diagonal to the left. If move does 
     *             not correspond to one of the pieces, returns 0.
     */
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

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param      chess  The chess object is created
     * @param[in]  enemy  True if king color differs from piece color, else false
     *
     * @return     Returns the coordinate position of the king, based on the current board
     *             representation and color determined by 'enemy'.
     */
    int findKingPos(int src, Chess * chess, bool enemy)
    {
        Piece* temp;
        for(const auto & elem : chess->getBoard())
        {
            if( (enemy && elem->isKing() && !elem->isSameColor(src, chess) ) ||
                ( !enemy && elem->isKing() && elem->isSameColor(src, chess) ) )
            {
                temp = elem;
            }
        } 

        return temp->getPieceSquare();
    }
} // unnamed namespace

/*************************************************************************************/
/*                              GLOBAL FUNCTIONS / OBJECTS                           */
/*************************************************************************************/
namespace chessCAMO
{
    /**
     * @brief      Iterates through the pieces on a current board representation to 
     *             produce the board on the console screen
     *
     * @param[in]  board  The board representation
     * 
     * \post
     * Each piece of the current board representation is printed to the
     * screen using a corresponding letter inside a formatted board
     */
    void printBoard(const vector<Piece*> & board)
    {
        char piece_char;
        char ranks[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
        
        int count = 0;
        for(const auto & elem : board)
        {
            if(count == 0)
            {
                cout << "    A   B   C   D   E   F   G   H" << endl;
            }

            if(count % 8 == 0)
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
                cout << ranks[count/8] << endl;
            if(count == 63)
            {
                cout << "  +---+---+---+---+---+---+---+---+" << endl;
                cout << "    A   B   C   D   E   F   G   H" << endl;
            }
            count++;
        }
    }

    /**
     * @brief      At any moment, the players can either continue, draw, or resign
     *
     * @param      chess  The chess object is created
     * @param      in     Input stream is selected (stdin or file)
     * 
     * \pre
     * None
     * 
     * \post
     *  Depending on the users choice, the program either continues
     * ('y' || 'd' + 'n') or terminates ('d' + 'y' || 'r')
     */
    void drawOrResign(Chess *chess, istream &in)
    {
        char user_input, draw_reply;
        string message;

        chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw, u -> undo move] ", PINK);
        in >> user_input;
        in.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)

        // error check
        while( std::tolower(user_input) != 'y' && std::tolower(user_input) != 'd' && std::tolower(user_input) != 'r' && std::tolower(user_input) != 'u' )
        {
            chessCAMO::printMessage("Pick one of the choices... try again!", YELLOW);
            chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw, u -> undo move] ", PINK);

            in >> user_input; // get new input
            in.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)
        }

        if(std::tolower(user_input) == 'r')
        {
            message = chess->getTurn() == WHITE ? "\nWhite resigned -> Black wins\n" 
                                                : "\nBlack resigned -> White wins\n";
            chessCAMO::printMessage(message, CYAN);
            chess->setCheckmate(true); // to end the game
            return;
        }
        else if(std::tolower(user_input) == 'd')
        {
            chessCAMO::printMessage("\nOffered draw... do you accept? [y -> yes, n -> no] ", PINK);
            in >> draw_reply;
            in.ignore(100, '\n'); // ignore rest of the previous input

            // error check
            while(std::tolower(draw_reply) != 'y' && std::tolower(draw_reply) != 'n')
            {
                chessCAMO::printMessage("Pick one of the choices... try again! ", YELLOW);
                chessCAMO::printMessage("\nOffered draw... do you accept? [y -> yes, n -> no] ", PINK);
                in >> draw_reply; // get new input
                in.ignore(100, '\n'); // ignore rest of the previous input
            }

            if(std::tolower(draw_reply) == 'y')
            {
                chessCAMO::printMessage("\nGame drawn by agreement\n", CYAN);
                chess->setCheckmate(true); // to end the game
                return;
            }
            else // std::tolower(draw_reply) == 'n'
            {
                chessCAMO::printMessage("\nDraw rejected. Game continues...\n", CYAN);
                return;
            }
        }
        else if(std::tolower(user_input) == 'u')
        {
            vector<int> squares_prior(64);
            vector<bool> moved_prior(64);
            vector<pair<bool, bool>> enpassant_prior(64);
            vector<Piece*> board = chess->getBoard();

            // store relevant piece information
            chess->storeOrRestore(board, squares_prior, moved_prior, enpassant_prior, "store");

            // undo the move
            chess->undoMove(board, squares_prior, moved_prior, enpassant_prior);

            // switch turn back to same player
            chess->setTurn(chess->getTurn() == WHITE ? BLACK : WHITE);

            // re-print board and display move information
            cout << "___________________________________________________" << endl;
            if(chess->getTurn() == WHITE)
                chessCAMO::printMessage("\n            White's move\n", CYAN);
            else
                chessCAMO::printMessage("\n            Black's move\n", CYAN);

            chessCAMO::printBoard(board);
        }
        else // player wants to continue
        {
            return ; // do nothing
        }
    }

    /**
     * @brief      Prints the given message ('text') with a given 'color' to console
     *
     * @param[in]  text   The text message to be created
     * @param[in]  color  One of the defined values at the top of the file
     * 
     * \pre
     * None
     * 
     * \post
     * The message is printed to the screen with color chosen and then
     * the color is changed back to default prior to return
     */
    void printMessage(string text, int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        cout << text;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
    }
}