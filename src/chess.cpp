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
 * @brief      Overloaded extraction operator
 *
 * @param      out           The output type (ex. ofstream or cout)
 * @param[in]  chess_object  The chess object
 *
 * @return     The output stream type
 */
ostream & operator << (ostream &out, const Chess &chess_object)
{
    for(const auto & elem : chess_object.getBoard())
        out << elem->getPieceType() << endl << elem->getPieceSquare() << endl << elem->getPieceColor() << endl
            << elem->getPieceMoveInfo() << endl << elem->getEnPassantLeft() << endl << elem->getEnPassantRight() << endl;

    out << chess_object.getCheck() << endl << chess_object.getDoubleCheck() << endl << chess_object.getCheckmate() << endl << chess_object.getStalemate() << endl;

    for(const auto & elem : chess_object.getReservoir())
        out << elem.first << elem.second << endl;

    out << chess_object.getTurn() << endl;

    return out;
}

/**
 * @brief      Overloaded insertion operator
 *
 * @param      in            The input type (ex. ifstream or cin)
 * @param      chess_object  The chess object
 *
 * @return     The input stream type
 */
istream & operator >> (istream &in, Chess &chess_object)
{
    // delete the allocated memory and restore new data
    vector<Piece*> board = chess_object.getBoard();
    for(auto & elem : board)
        delete elem; // GCOVR_EXCL_LINE

    string input;
    for(auto & elem : board)
    {
        in >> input;
        switch(input[0] - '0')
        {
            case 0:
                elem = new Pawn(0, PAWN, NEUTRAL);
                break;
            case 1:
                elem = new Knight(0, KNIGHT, NEUTRAL);
                break;
            case 2:
                elem = new Bishop(0, BISHOP, NEUTRAL);
                break;
            case 3:
                elem = new Rook(0, ROOK, NEUTRAL);
                break;
            case 4:
                elem = new Queen(0, QUEEN, NEUTRAL);
                break;
            case 5:
                elem = new King(0, KING, NEUTRAL);
                break;
            default:
                elem = new Empty(0, EMPTY, NEUTRAL);    
        }

        in >> input;
        elem->setPieceSquare(stoi(input));
        in >> input;
        elem->setPieceColor((pieceColor) (input[0] - '0'));
        in >> input;
        elem->setPieceMoveInfo(input == "1");
        in >> input;
        elem->setEnPassantLeft(input == "1");
        in >> input;
        elem->setEnPassantRight(input == "1");
    }

    chess_object.setBoard(board);

    in >> input;
    chess_object.setCheck(input == "1");
    in >> input;
    chess_object.setDoubleCheck(input == "1");
    in >> input;
    chess_object.setCheckmate(input == "1");
    in >> input;
    chess_object.setStalemate(input == "1");

    vector<pair<int, char>> currernt_reservoir(10);
    for(int i = 0; i < 10; i++)
    {
        in >> input;
        currernt_reservoir[i] = std::make_pair(input[0] - '0', input[1]);   
    }
    
    chess_object.setReservoir(currernt_reservoir);

    in >> input;
    chess_object.setTurn((pieceColor) (input[0] - '0'));

    return in;
}

/**
 * @brief      This anonymous namespace contains the local functions related to
 *             chessCAMO which are mainly used as helper functions to determine
 *             critical information regarding a given move choice.
 */
namespace
{
    /**
     * @brief      Determines if a destination square is in the pinning path as
     *             sometimes you can simply move the pinned piece closer to the
     *             pinning piece.
     *
     * @param[in]  src   The source square of pinned piece
     * @param[in]  dest  The destination square of pinned piece
     * @param[in]  pin   Source square of pinning piece
     *
     * @return     Allows the move (returns false) if piece can move closer to
     *             pinning piece, else move is invalid (returns true)
     */
    bool destNotInPinPath(int src, int dest, int pin);

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same column
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     True if the source and destination are in the same column,
     *             else False.
     */
    bool sameCol(int src, int dest);

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same row
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     True if the source and destination are in the same row, else
     *             False.
     */
    bool sameRow(int src, int dest);

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same diagonal
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
     * @return     If only 1 piece is in the path from 'src' to 'dest', return
     *             its coordinates else, return -1 to indicate that there is
     *             either no piece or multiple pieces in the path.
     */
    int squareOfPieceInPath(int src, int dest, const Chess &chess);

    /**
     * @brief      Used to determine the increment to use when moving a piece
     *             from 'src' to 'dest'.
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     Returns either 1 if moving in same row, 7 if moving in
     *             diagonal to the right, 8 if moving in same column, 9 if
     *             moving in diagonal to the left. If move does not correspond
     *             to one of the pieces, returns 0.
     */
    int incrementChoice(int src, int dest);

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param      chess  The chess object is created
     * @param[in]  enemy  True if king color differs from piece color, else
     *                    false
     *
     * @return     Returns the coordinate position of the king, based on the
     *             current board representation and color determined by 'enemy'.
     */
    int findKingPos(int src, const Chess &chess, bool enemy); 

    /**
     * @brief      Gets the path to the object_states folder.
     *
     * @param[in]  num_moves  The number of moves made on the board
     *
     * @return     The path to the object_states folder.
     *
     * @note       The return value depends on target of the global makefile
     */
    string getPath(int num_moves);    
} // unnamed namespace (makes these functions local to this implementation file)

/*************************************************************************************/
/*                              CHESS CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      Default constructor with default board parameter initialization -
 *             Constructs a new instance.
 */
Chess::Chess()
    : board(64), check_pieces(2), flags(4), reservoir(10), turn{WHITE}, num_moves{0} 
{
    for(int i = 0; i < 10; i++)
    {
        if(i == 0 || i == 5)
            reservoir[i] = i == 0 ? std::make_pair(4, 'p') :  std::make_pair(4, 'P');
        else if(i == 1 || i == 6)
            reservoir[i] = i == 1 ? std::make_pair(2, 'n') :  std::make_pair(2, 'N');
        else if(i == 2 || i == 7)
            reservoir[i] = i == 2 ? std::make_pair(2, 'o') :  std::make_pair(2, 'O'); // bishop
        else if(i == 3 || i == 8)
            reservoir[i] = i == 3 ? std::make_pair(1, 'r') :  std::make_pair(1, 'R');
        else
            reservoir[i] = i == 4 ? std::make_pair(1, 'q') :  std::make_pair(1, 'Q');
    }
}

/**
 * @brief      Destroys the object and frees any dynamically allocated
 *             memory ('new') to avoid memory leaks.
 *
 * @note       This is needed since a game can end before a corresponding
 *             checkmate/stalemate flag is reached and thus 'board' and
 *             'check_pieces' will have dynamically allocated memory that
 *             isn't freed.
 */
Chess::~Chess()
{
    for(auto & elem : board)
        delete elem; // GCOVR_EXCL_LINE

    for(auto & elem : check_pieces)
        delete elem; // GCOVR_EXCL_LINE
}

/**
 * @brief      Places the pieces on the board at their correct starting
 *             positions
 *
 * @pre        The chess object is intialized
 *
 * @post       Instantiates new objects corresponding to the pieces, places them
 *             in the corresponding index of the board vector and set the global
 *             object's board variable
 */  
void Chess::boardInit()
{
    // initialize the board
    for(unsigned int i = 0; i < board.size(); i++)
    {
        /******************* BLACK *******************/
        if(i < board.size()/4)          // first 2 rows
        {
            if(i == 0 || i == 7)        // rook
                board[i] = new Rook(i, ROOK, BLACK);
            else if(i == 1 || i == 6)   // knight
                board[i] = new Knight(i, KNIGHT, BLACK); 
            else if(i == 2 || i == 5)   // bishop
                board[i] = new Bishop(i, BISHOP, BLACK); 
            else if(i == 3)             // queen
                board[i] = new Queen(i, QUEEN, BLACK);
            else if(i == 4)             // king
                board[i] = new King(i, KING, BLACK);
            else                        // pawn
                board[i] = new Pawn(i, PAWN, BLACK);
        }

        /********** NEUTRAL (EMPTY SQUARES) **********/
        else if(i < board.size()*3/4)    
            board[i] = new Empty(i, EMPTY, NEUTRAL); // middle 4 rows

        /***************** WHITE *******************/
        else                                // last 2 rows
        {
            if(i == 56 || i == 63)          // rook
                board[i] = new Rook(i, ROOK, WHITE);
            else if(i == 57 || i == 62)     // knight
                board[i] = new Knight(i, KNIGHT, WHITE); 
            else if(i == 58 || i == 61)     // bishop
                board[i] = new Bishop(i, BISHOP, WHITE);
            else if(i == 59)                // queen
                board[i] = new Queen(i, QUEEN, WHITE);
            else if(i == 60)                // king
                board[i] = new King(i, KING, WHITE);
            else                            // pawn
                board[i] = new Pawn(i, PAWN, WHITE);
        }
    }

    check_pieces[0] = new Empty(0, EMPTY, NEUTRAL);
    check_pieces[1] = new Empty(0, EMPTY, NEUTRAL);

    // printing the board and letting user know whose turn it is
    // white always starts first in chess!
    chessCAMO::printBoard(getBoard(), getReservoir());
    chessCAMO::printFooterMessage("'s move", *this);

    // serializing the object to a file for later re-use
    chessCAMO::saveObject(getNumMoves(), *this);
}

/**
 * @brief      Moves a piece on the board from 'src' to 'dest' if conditions for
 *             a legal move are met.
 *
 * @param[in]  src   The source square (piece's current location)
 * @param[in]  dest  The destination square (piece's ending location)
 * @param      in    input stream type (stdin or file)
 *
 * @pre        The chess object is created.
 *
 * @post       The pieces at 'src' and 'dest' positions are swapped. If needed
 *             (attacking, castling, etc.) an empty square is made. The board's
 *             state is updated to indicate that the move occured. On failure,
 *             an error message is printed and user is asked to retry.
 *             
 * @return     True if move was made, False otherwise.
 */
bool Chess::makeMove(int src, int dest, istream &in)
{   
    // first check to see if reservoir is used
    // if so, there is nothing to do as everything is handled there
    // else, check regular chess functionality
    if( ( 0 <= src && src <= 63 && board[src]->isLegalMove(dest, *this) && board[src]->getPieceColor() == getTurn()) || 
        ( 110 <= src && src <= 114 && useReservoirPiece(src, dest) ) )
    {
        vector<Piece*> board; 
        vector<Piece*> check_pieces = getCheckPieces();

        chessCAMO::restoreObject(getNumMoves(), *this);

        // make the appropriate move from 'src' to 'dest' (if not using piece reservoir)
        if(src <= 63)
            makeMoveForType(src, dest);

        if(getDoubleCheck() || getCheck())
        {    
            // When in double check you must move the king. When in single check
            // you have three choices: move the king, defend the path, attack
            // the checking piece. If the move failed, undo the board
            // representation and do not set check_pieces
            if( needUndoMove(check_pieces[0]) )
            {
                // restore previous object
                chessCAMO::restoreObject(getNumMoves(), *this);
                
                chessCAMO::printBoard(getBoard(), getReservoir());

                cout << "___________________________________________________" << endl;
                if(getCheck())
                    chessCAMO::printMessage("You are in check! Try again...\n", YELLOW);
                else
                    chessCAMO::printMessage("You are in double check! Try again...\n", YELLOW);

                return false;
            }

            // did not return so set the appropriate member variables
            setDoubleCheck(false);
            setCheck(false);
        }

        board = getBoard();

        // en-passant checking/updating
        board[dest]->enPassantHandling(src, *this); 

        // pawn promotion
        if(dest/8 == 0 || dest/8 == 7)
            board[dest]->promotePawn(*this, in);

        // did the move cause a double check?
        if(board[src]->causeDoubleCheck(dest, *this)) 
            isCheckmate("double"); // .. and for checkmate

        // did the move cause a check?
        else if(board[src]->causeCheck(dest, *this)) 
            isCheckmate("single"); // .. and for checkmate

        // check for stalemate
        if(isStalemate()) 
            handleStalemate();

        // after a move was made and all the above checks passed, can finally change the turn
        handleChangeTurn();

        // increment move counter by 1 since a move was made (if not used the reservoir)
        src <= 63 ? setNumMoves(getNumMoves()+1) : setNumMoves(getNumMoves());

        // save the object in the corresponding file
        chessCAMO::saveObject(getNumMoves(), *this);

        return true;
    }
    else
    {
        chessCAMO::printBoard(getBoard(), getReservoir());

        if(getDoubleCheck())
            chessCAMO::printMessage("\nYou must move your king!\n", YELLOW);
        else
            chessCAMO::printMessage("\nInvalid move! Try again...\n", YELLOW);

        chessCAMO::printFooterMessage("'s move", *this);

        return false;
    }
}

/**
 * @brief      At any turn, a player can replace one of their pieces with a
 *             piece from the reservoir if the resources are available
 *
 * @param[in]  src   The piece's source square (this will be an ASCII code in
 *                   [110, 114] depending on the character the user enters)
 * @param[in]  dest  The piece's destination square (piece that will be replaced
 *                   on the board)
 *
 * @return     True if replacement is applied, False otherwise.
 */
bool Chess::useReservoirPiece(int src, int dest)
{
    chessCAMO::restoreObject(getNumMoves(), *this);

    vector<Piece*> current_board = getBoard();
    vector<pair<int, char>> current_reservoir = getReservoir();
    pieceColor current_turn = getTurn();

    // if the piece you want to replace matches your color and your replacement
    // piece is not of the same type, then go ahead. 'src' must be in [110, 114]
    // which is ascii values of the reservoir pairs. Note cannot replace king or
    // use reservoir when in check/double check
    if(current_board[dest]->getPieceColor() == current_turn && !current_board[dest]->isKing() && !getCheck() && !getDoubleCheck())
    {
        for(int i = 0; i < 10; i++)
        {
            if(current_reservoir[i].first > 0 && (int) std::tolower(current_reservoir[i].second) == src &&
              ( (std::isupper(current_reservoir[i].second) && current_turn == WHITE) || (islower(current_reservoir[i].second) && current_turn == BLACK) ) )
            {
                // decrement the piece reservoir count accordingle
                current_reservoir[i].first -= 1;

                // replace the piece on the board
                switch( std::tolower(current_reservoir[i].second) )
                {
                    case 'q':
                        if(current_board[dest]->isQueen())
                            return false;
                        else
                        {
                            delete current_board[dest]; // GCOV_EXCL_LINE
                            current_board[dest] = new Queen(dest, QUEEN, getTurn());
                        }
                        break;
                    case 'r':
                        if(current_board[dest]->isRook())
                            return false;
                        else
                        {
                            delete current_board[dest]; // GCOV_EXCL_LINE
                            current_board[dest] = new Rook(dest, ROOK, getTurn());
                        }
                        break;
                    case 'o': // bishop
                        if(current_board[dest]->isBishop())
                            return false;
                        else
                        {
                            delete current_board[dest]; // GCOV_EXCL_LINE
                            current_board[dest] = new Bishop(dest, BISHOP, getTurn());
                        }
                        break;
                    case 'n':
                        if(current_board[dest]->isKnight())
                            return false;
                        else
                        {
                            delete current_board[dest]; // GCOV_EXCL_LINE
                            current_board[dest] = new Knight(dest, KNIGHT, getTurn());
                        }
                        break;
                    default: // pawn 'p' (cannot be placed in a promoting square)
                        if(current_board[dest]->isPawn() || dest/8 == 0 || dest/8 == 7)
                            return false;
                        else
                        {
                            delete current_board[dest]; // GCOV_EXCL_LINE
                            current_board[dest] = new Pawn(dest, PAWN, getTurn());
                        }
                }

                // since the piece is brand new, can set its relevant values
                current_board[dest]->setEnPassantLeft(false);
                current_board[dest]->setEnPassantRight(false);
                current_board[dest]->setPieceMoveInfo(true); 
                
                // update the board and reservoir as needed
                setBoard(current_board);
                setReservoir(current_reservoir);

                // increment move counter by 1 since a move was made 
                setNumMoves(getNumMoves()+1);

                // save the object in the corresponding file
                chessCAMO::saveObject(getNumMoves(), *this);

                return true;
            }
        }
    }

    return false;  // default return value
}

/**
 * @brief      Decide if a move caused a checkmate according to 'check_type'
 *
 * @param[in]  check_type  Either "single" or "double" corresponding to the
 *                         check on the board
 *
 * @pre        The chess object is created.
 *
 * @post       If board's state is in checkmate, calls Chess::handleCheckmate()
 *             to print messages to console indicating the winner. Else, game
 *             continues as usual.
 */
void Chess::isCheckmate(string check_type)
{
    vector<Piece*> check_pieces = getCheckPieces();
    
    if( (check_type == "double" && doubleCheckPieceIterator(check_pieces[1])) ||
        (check_type == "single" && singleCheckPieceIterator(check_pieces[0], check_pieces[1])) )
    { 
        handleCheckmate();
    }   
    else
    {
        chessCAMO::printBoard(getBoard(), getReservoir());

        if(getCheck())
            chessCAMO::printMessage("\nCheck!\n", CYAN);
        else
            chessCAMO::printMessage("\nDouble Check!\n", CYAN);
    }
}

/**
 * @brief      Decide if a move caused a stalemate
 * 
 * @pre        The chess object is created.
 *
 * @post       Calls Chess::handleStalemate() to print messages to console
 *             indicating that game is drawn, if needed.
 *
 * @return     True if board's state is in stalemate, else False and game
 *             continues as usual.
 */
bool Chess::isStalemate()
{
    vector<Piece*> board = getBoard();

    // this is a list of all the possible increments (moves) a piece on the board could have
    int possible_moves[22] = {1, 6, 7, 8, 9, 10, 14, 15, 16, 17, 18,
                              21, 27, 28, 35, 36, 42, 45, 49, 54, 56, 63};
    
    for(const auto & elem : board)
    {
        if(elem->getPieceColor() == switchTurn())
        {
            for(int move : possible_moves)
            {
                if( elem->isLegalMove(elem->getPieceSquare() + move, *this) ||
                    elem->isLegalMove(elem->getPieceSquare() - move, *this) )
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
 * @pre        The chess object is created
 *
 * @post       Swaps the pieces on the board according to 'src' and 'dest' and
 *             proper chess rules, using Chess::pieceSwap(int src, int dest,
 *             vector<Piece*> & board). If a new empty square must be created,
 *             this is handled. Returns board representation with the made move.
 */
void Chess::makeMoveForType(int src, int dest)
{
    vector<Piece*> board = getBoard();

    // castling move
    if(board[src]->canCastle(dest, *this))
    {
        // note that the pieces are moved
        board[src]->setPieceMoveInfo(true);
        board[dest]->setPieceMoveInfo(true);

        if(std::abs(src - dest) == 3) // king side castle
        {
            pieceSwap(src, src + 2, board); // king move
            pieceSwap(dest, dest - 2, board); // rook move
        }

        else // std::abs(src-dest) == 4 -> queen side castle
        {
            pieceSwap(src, src - 2, board); // king move
            pieceSwap(dest, dest + 3, board); // rook move
        }
    }

    // en-passant move
    else if((board[src]->getEnPassantLeft() || board[src]->getEnPassantRight()) && std::abs(src-dest) != 8)
    {
        int sign = board[src]->isPieceWhite() ? 1 : -1;

        // delete the pawn that caused en-passant (make it an empty square)
        if(std::abs(src-dest) == 7 && board[src]->getEnPassantRight())
        {
            delete board[src+sign]; // GCOVR_EXCL_LINE
            board[src+sign] = new Empty(src+sign, EMPTY, NEUTRAL);
        }
        else if(std::abs(src-dest) == 9 && board[src]->getEnPassantLeft())
        {
            delete board[src-sign]; // GCOVR_EXCL_LINE
            board[src-sign] = new Empty(src-sign, EMPTY, NEUTRAL);
        }
    }

    // regular or attacking
    else { board[src]->setPieceMoveInfo(true); } // note that the piece moved

    // make the move, delete the source square in case have not made
    // en-passant move (since can choose not to take with en-passant)
    pieceSwap(src, dest, board);
    delete board[src]; // GCOVR_EXCL_LINE
    board[src] = new Empty(src, EMPTY, NEUTRAL);

    setBoard(board);
}

/**
 * @brief      Used in Chess::makeMoveForType(int src, int dest) to swap pieces
 *             on the board
 *
 * @param[in]  src    The source square of piece
 * @param[in]  dest   The destination square of piece
 * @param      board  The current board representation
 *
 * @pre        The chess object is created
 *
 * @post       Swaps the pieces on the board according to 'src' and 'dest'.
 */
void Chess::pieceSwap(int src, int dest, vector<Piece*> &board)
{
    board[src]->setPieceSquare(dest);
    board[dest]->setPieceSquare(src);
    std::swap(board[src], board[dest]);
}

/**
 * @brief      Indicates who will move next via a message to console
 *
 * @pre        The chess object is created
 *
 * @post       Player turn is switched, board is printed, and message is
 *             displayed if game is not over to indicate whose turn it is.
 */
void Chess::handleChangeTurn()
{
    setTurn(switchTurn());

    if(!getCheck() && !getDoubleCheck())
        chessCAMO::printBoard(getBoard(), getReservoir());

    if(!getCheckmate() && !getStalemate())
        chessCAMO::printFooterMessage("'s move", *this);
}

/**
 * @brief      Indicates which player won by checkmate via a message to console
 *
 * @pre        The chess object is created. A move was made (cannot checkmate in
 *             less than 2 moves in theory).
 *
 * @post       Object's checkmate state is set to true (to end the algorithm)
 */
void Chess::handleCheckmate()
{
    chessCAMO::printBoard(getBoard(), getReservoir());
    chessCAMO::printFooterMessage(" won by Checkmate!\n", *this);
    setCheckmate(true);
}

/**
 * @brief      Indicates the game is drawn via a message to console
 * @pre        The chess object is created. A move was made.
 *
 * @post       Object's stalemate state is set to true (to end the algorithm)
 */
void Chess::handleStalemate()
{
    chessCAMO::printBoard(getBoard(), getReservoir());
    chessCAMO::printFooterMessage(" won by Checkmate!\n", *this);
    setStalemate(true);
}

/**
 * @brief      After a move is made, can undo it if move was invalid and return
 *             to previous board state
 *
 * @param      piece  The piece that is attacking the king currently
 *
 * @pre        The chess object is created. A move was made.
 *
 * @post       None
 *
 * @return     True if after move, the 'king' is still in check (single or
 *             double) or the move was invalid, output warning message and undo
 *             the move. Else, False and continue the game without undoing the
 *             move.
 */
bool Chess::needUndoMove(Piece *piece)
{
    // move was already made, so check if the piece can still attack the king
    // while making sure that the king did not move to or away from the piece
    // (its square will be empty if so)
    vector<Piece*> board = getBoard();

    int king_pos_single = findKingPos(piece->getPieceSquare(), *this, true);  // opposite color
    int king_pos_double = findKingPos(piece->getPieceSquare(), *this, false); // same color

    if(getCheck())
    {
        switchTurn(); // player turn was not switched yet even though move was made
        return piece->isPossibleMove(king_pos_single, *this);
    }
    else // getDoubleCheck()
    {
        for(const auto & elem : board)
        {
            if(elem->isPossibleMove(king_pos_double, *this))
                return true;
        }

        return false;
    }
} 
       
/**
 * @brief      If in a single check, see if piece can defend the king, capture
 *             attacking piece, or move the king out of check. Used in
 *             Chess::isCheckmate("single")
 *
 * @param      piece  The piece that is attacking the king currently
 * @param      king   The king that is being attacked currently
 *
 * @pre        The chess object is created. A move was made.
 *
 * @post       None
 *
 * @return     True if no legal moves found (checkmate), else False and make the
 *             move
 */
bool Chess::singleCheckPieceIterator(Piece *piece, Piece *king)
{
    vector<Piece*> board = getBoard();
    int increment, src = piece->getPieceSquare(), dest = king->getPieceSquare();

    // can a piece defend the king from check?
    if(!piece->isKnight())
    {
        increment = src > dest ? -1*incrementChoice(src, dest) : incrementChoice(src, dest);
        for(const auto & elem : board)
        {
            if(elem->isSameColor(dest, *this) && !elem->isKing())
                for(int pos = src; pos != dest; pos += increment)   
                    if(elem->isPossibleMove(pos, *this))
                        return false;
        }
    }
    else // for a Knight check, to defend the king a piece must take the knight
    {
        for(const auto & elem : board)
            if(!elem->isKing() && elem->isSameColor(dest, *this) && elem->isPossibleMove(src, *this))
                return false;
    }

    // can king move out of check?
    return doubleCheckPieceIterator(king);
}

/**
 * @brief      If in a double check, see if the king can move out of check as
 *             this is the only valid move option. Used in
 *             Chess::isCheckmate("double").
 *
 * @param      king  The king that is being attacked currently
 *
 * @pre        The chess object is created.
 *
 * @post       None
 *
 * @return     True if no legal moves found (checkmate), else False and make the
 *             move
 */
bool Chess::doubleCheckPieceIterator(Piece *king)
{
    int src = king->getPieceSquare();
    int king_moves[4] = {1, 7, 8, 9}; // possible king moves (absolute values here)

    // can king move out of check?
    for(int move : king_moves)
    {
        if(king->isLegalMove(src + move, *this) || king->isLegalMove(src - move, *this))
            return false;
    }
        
    return true; // no legal moves found ? true : false
}

/**
 * @brief      Decides whose turn it is currently and updates the private member
 *             variable ('turn') accordingly
 *
 * @pre        The chess object is created.
 *
 * @post       None
 *
 * @return     'chess.turn' is set to the correct player
 */
pieceColor Chess::switchTurn() { return getTurn() == WHITE ? BLACK : WHITE; }

/*************************************************************************************/
/*                              PIECE CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      Determines if 2 pieces have the same color
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if source piece color matches destination piece color, False
 *             otherwise.
 */
bool Piece::isSameColor(int dest, const Chess &chess)
{

    vector<Piece*> board = chess.getBoard();

    // cannot use getPieceColor() here since the board might be updated
    return board[getPieceSquare()]->getPieceColor() == board[dest]->getPieceColor();
}

/**
 * @brief      Determines if a given piece is pinned to the king by opposing
 *             piece
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if piece is pinned to the king and moving to 'dest' will
 *             cause the path (pinning piece -> king from pinned piece side) to
 *             be free, False otherwise.
 */
bool Piece::isPinned(int dest, const Chess &chess)
{
    int king_pos, src = getPieceSquare();
    vector<Piece*> board = chess.getBoard();

    if(!isKing())
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
 * @return     True if squares along the path (src, dest) are empty, False
 *             otherwise.
 */
bool Piece::isPathFree(int dest, const Chess &chess)
{
    int increment, src = getPieceSquare();
    vector<Piece*> board = chess.getBoard();

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
    else { return false; }
}

/**
 * @brief      Determines if a move is legal based on the rules of chess
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if moving the piece to 'dest' is legal from any type of move
 *             and piece, False otherwise.
 *
 * @note       A possible move, is not necessarily legal.
 */
bool Piece::isLegalMove(int dest, Chess &chess)
{
    if(chess.getDoubleCheck() && !isKing())
        return false;
    else
        return 0 <= dest && dest <= 63 && getPieceSquare() != dest && 
               isPossibleMove(dest, chess) && !isPinned(dest, chess) && !movedIntoCheck(dest, chess);
}

/**
 * @brief      Did the move cause a check?
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if moving the piece to 'dest' now threatens the opposing
 *             king. False otherwise.
 */
bool Piece::causeCheck(int dest, Chess &chess)
{
    vector<Piece*> check_pieces = chess.getCheckPieces();
    vector<Piece*> board = chess.getBoard();
    
    int king_pos = findKingPos(dest, chess, true); // opposite color king position

    if(board[dest]->isPossibleMove(king_pos, chess))
    {
        // push the king and checking piece onto the stack and set the corresponding 
        // object variables (checkStack and setCheck)
        delete check_pieces[0]; // GCOVR_EXCL_LINE
        delete check_pieces[1]; // GCOVR_EXCL_LINE

        switch(board[dest]->getPieceType())
        {
            case 0:
                check_pieces[0] = new Pawn(dest, PAWN, board[dest]->getPieceColor());
                break;
            case 1:
                check_pieces[0] = new Knight(dest, KNIGHT, board[dest]->getPieceColor());
                break;
            case 2:
                check_pieces[0] = new Bishop(dest, BISHOP, board[dest]->getPieceColor());
                break;
            case 3:
                check_pieces[0] = new Rook(dest, ROOK, board[dest]->getPieceColor());
                break;
            default:
                check_pieces[0] = new Queen(dest, QUEEN, board[dest]->getPieceColor());
        }

        check_pieces[1] = new King(king_pos, KING, board[king_pos]->getPieceColor());
        chess.setCheckPieces(check_pieces);
        chess.setCheck(true);
    } 

    return chess.getCheck();
}

/**
 * @brief      Did the move cause a double check?
 *
 * @param[in]  dest   The destination square of the piece
 * @param      chess  The chess object
 *
 * @return     True if moving the piece to 'dest' now threatens the opposing
 *             king, and an additional piece from the same side also has a legal
 *             move towards the opposing king. False otherwise.
 */
bool Piece::causeDoubleCheck(int dest, Chess &chess)
{
    int king_pos, checking_piece_counter = 0;
    vector<Piece*> board = chess.getBoard();
    vector<Piece*> check_pieces = chess.getCheckPieces();

    king_pos = findKingPos(dest, chess, true); // opposite color king position
    
    // how many pieces are checking the king
    for(const auto & elem : board)
    {
        if(elem->isLegalMove(king_pos, chess))
            checking_piece_counter++;
    }

    // double check if 2 pieces are attacking the king
    if(checking_piece_counter == 2)
    {
        delete check_pieces[0]; // GCOVR_EXCL_LINE
        delete check_pieces[1]; // GCOVR_EXCL_LINE

        // make the king last in the vector with both pieces being identical
        check_pieces[0] = new King(king_pos, KING, board[king_pos]->getPieceColor()); 
        check_pieces[1] = new King(king_pos, KING, board[king_pos]->getPieceColor()); 

        chess.setCheckPieces(check_pieces);
        chess.setDoubleCheck(true);
        return true;
    }

    return chess.getDoubleCheck();
}

/*************************************************************************************/
/*                              PAWN CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * @note       Can move 1 or 2 square (if not moved yet) forwards, attack
 *             diagonally 1 square, en-passant, and promote.
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The piece's destination square
 * @param      chess  The chess object
 *
 * @return     True if possible move, False otherwise.
 */
bool Pawn::isPossibleMove(int dest, const Chess &chess)
{
    vector<Piece*> board = chess.getBoard();

    bool legal = false;
    int src = getPieceSquare();
    int diff = isPieceWhite() ? src-dest : dest - src;

    // on attack it can move diagonally, first move can be 2 squares forward,
    // en-passant is possible for one move if conditions are met
    if(getPieceMoveInfo())
    {
        legal = (diff == 8 && board[dest]->isEmpty()) || 
                ( ( diff == 7 && ( !board[dest]->isEmpty() || board[src]->getEnPassantRight() ) ) ||
                  ( diff == 9 && ( !board[dest]->isEmpty() || board[src]->getEnPassantLeft() ) ) );
    }
    else // cannot en-passant if you have not moved yet
    {
        legal = ((diff == 8 || diff == 16) && board[dest]->isEmpty()) ||
                ((diff == 7 || diff == 9) && !board[dest]->isEmpty());
    }

    return legal && isPathFree(dest, chess) && !isSameColor(dest, chess);
}

/**
 * @see        virtual Piece::enPassantHandling(int dest, Chess &chess)
 *
 * @brief      Captures the violating pawn if an en-passant move it made.
 *
 * @param[in]  src    The piece's source square
 * @param      chess  The chess object
 */
void Pawn::enPassantHandling(int src, Chess &chess)
{
    vector<Piece*> board = chess.getBoard();
    int dest = getPieceSquare();
    int sign = isPieceWhite() ? 1 : -1;

    // First, cancel en-passant abilities of all pawns. Then determine which
    // pawn can have en-passant abilities
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
 * @see        virtual Piece::promotePawn(int dest, Chess &chess, istream
 *             &in)
 *
 * @brief      Promotes the pawn to a piece depending on the user's choice.
 *
 * @param      chess  The chess object
 * @param      in     The input stream type (ex. ifstream or cin)
 */
void Pawn::promotePawn(Chess &chess, istream &in)
{
    vector<Piece*> board = chess.getBoard();
    bool white_turn = chess.getTurn() == WHITE;

    char piece;
    int dest = getPieceSquare();

    while(true)
    {
        chessCAMO::printMessage("Which Piece: Q/q | R/r | B/b | N/n? ", PINK);

        in >> piece;
        
        if(std::tolower(piece) == 'q')
        {
            delete board[dest]; // GCOVR_EXCL_LINE
            board[dest] = white_turn ? new Queen(dest, QUEEN, WHITE) : new Queen(dest, QUEEN, BLACK);
            break;
        }
        else if(std::tolower(piece) == 'r')
        {
            delete board[dest]; // GCOVR_EXCL_LINE  
            board[dest] = white_turn ? new Rook(dest, ROOK, WHITE) : new Rook(dest, ROOK, BLACK);
            break;
        }
        else if(std::tolower(piece) == 'b')
        {
            delete board[dest]; // GCOVR_EXCL_LINE 
            board[dest] = white_turn ? new Bishop(dest, BISHOP, WHITE) : new Bishop(dest, BISHOP, BLACK);
            break;
        }
        else if(std::tolower(piece) == 'n')
        {
            delete board[dest]; // GCOVR_EXCL_LINE
            board[dest] = white_turn ? new Knight(dest, KNIGHT, WHITE) : new Knight(dest, KNIGHT, BLACK);
            break;
        }
        else
            chessCAMO::printMessage("\nPick one of the choices\n", YELLOW);
    }

    chess.setBoard(board);
}

/*************************************************************************************/
/*                              KNIGHT CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
/**
 * @note       Can move (2 up/down or 2 left/right) and (1 left/right or 1
 *             up/down), can jump over pieces.
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if possible move, False otherwise.
 */
bool Knight::isPossibleMove(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    int diff = std::abs(src - dest);
    return std::abs(src/8 - dest/8) <= 2 && std::abs(src%8 - dest%8) <= 2 &&
           (diff == 6 || diff == 10 || diff == 15 || diff == 17) &&
           !isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              BISHOP CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/

/**
 * @note       Can move diagonally any number of squares
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if possible move, False otherwise.
 */
bool Bishop::isPossibleMove(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    int diff = std::abs(src - dest);
    return (diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest) &&
           isPathFree(dest, chess) && !isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              ROOK CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * @note       Can move horizontally or vertically any number of squares.
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if possible move, False otherwise.
 */
bool Rook::isPossibleMove(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    return ( sameRow(src, dest) || sameCol(src, dest) ) &&
           isPathFree(dest, chess) && !isSameColor(dest, chess);
}

/*************************************************************************************/
/*                              QUEEN CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @note       Combines rook and bishop moves.
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if possible move, False otherwise.
 */
bool Queen::isPossibleMove(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    int diff = std::abs(src - dest);
    return ( ((diff % 7 == 0 || diff % 9 == 0) && sameDiag(src, dest)) ||
             (sameRow(src, dest) || sameCol(src, dest)) ) &&
             isPathFree(dest, chess) && !isSameColor(dest, chess);
}

/*************************************************************************************/
/*                             KING CLASS - MEMBER FUNCTIONS                         */
/*************************************************************************************/
/**
 * @note       Combines rook and bishop moves but only 1 square
 *
 * @see        virtual Piece::isPossibleMove(int dest, Chess &chess)
 *
 * @brief      Determines if possible move.
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if possible move, False otherwise.
 */
bool King::isPossibleMove(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    int diff = std::abs(src - dest);
    int col_diff = std::abs(src%8 - dest%8), row_diff = std::abs(src/8 - dest/8);
    return ( (col_diff <= 1 && row_diff <= 1) && (diff == 1 || diff == 7 || diff == 8 || diff == 9) && !isSameColor(dest, chess) ) ||
           ( (diff == 3 || diff == 4) && canCastle(dest, chess) ) ;
}

 /**
  * @see        virtual Piece::canCastle(int dest, Chess &chess)
  *
  * @brief      Determines ability to castle.
  *
  * @param[in]  dest   The destination
  * @param      chess  The chess
  *
  * @return     True if able to castle, False otherwise.
  */
bool King::canCastle(int dest, const Chess &chess)
{
    int src = getPieceSquare();
    int increment = src > dest ? -1 : 1;

    vector<Piece*> board = chess.getBoard();

    if( getPieceMoveInfo() || board[dest]->getPieceMoveInfo() || chess.getCheck() || !board[dest]->isRook() )  { return false; }
    else
    {
        for(const auto & elem : board)
        {
            if(!elem->isEmpty() && !elem->isSameColor(src, chess))
            {
                // king only moves 2 squares regardless of castle direction
                for(int square = src+increment; square != src+(2*increment); square += increment)    
                    if(elem->isPossibleMove(square, chess))
                        return false;
            }
        }

        return isPathFree(dest, chess);
    }
}

/**
 * @see        virtual Piece::movedIntoCheck(int dest, Chess &chess)
 *
 * @brief      Determines if a king moved into check after a move was made
 *
 * @param[in]  dest   The destination
 * @param      chess  The chess
 *
 * @return     True if a piece attacks the king after the move was made,
 *             False otherwise.
 */
bool King::movedIntoCheck(int dest, Chess &chess)
{
    vector<Piece*> board = chess.getBoard();
    int src, sign;

    pieceColor original_color = board[dest]->getPieceColor();
    bool ret_val = false;

    for(const auto & elem : board)
    {
        src = elem->getPieceSquare();
        sign = elem->isPieceWhite() ? 1 : -1;

        // change the color of the attacking piece so that if it is supported,
        // one can check if the supporting piece can move into that square (in
        // which case the king cannot capture the attacker)
        board[dest]->setPieceColor(NEUTRAL);

        // pawn can only attack sideways, but the board isn't updated yet so it
        // will always be invalid move checking dest-increment and increment
        // since if a piece is only 1 square away dest-increment gives src=dest.
        // However, since move isn't made it is possible that a piece of same
        // color causes isPossibleMove(dest, chess) to be false, see
        // 48-scholarMate.txt
        if( !elem->isEmpty() && !isSameColor(src, chess) && 
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
     * @brief      Determines if a destination square is in the pinning path as
     *             sometimes you can simply move the pinned piece closer to the
     *             pinning piece.
     *
     * @param[in]  src   The source square of pinned piece
     * @param[in]  dest  The destination square of pinned piece
     * @param[in]  pin   Source square of pinning piece
     *
     * @return     Allows the move (returns false) if piece can move closer to
     *             pinning piece, else move is invalid (returns true)
     */
    bool destNotInPinPath(int src, int dest, int pin)
    {
        int inc_dest = incrementChoice(src, dest);
        int inc_pin = incrementChoice(src, pin);

        return !(inc_dest != 0 && inc_dest == inc_pin);
    }

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same column
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     True if the source and destination are in the same column,
     *             else False.
     */
    bool sameCol(int src, int dest) { return src % 8 == dest % 8; }

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same row
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     True if the source and destination are in the same row, else
     *             False.
     */
    bool sameRow(int src, int dest) { return src / 8 == dest / 8; }

    /**
     * @brief      Determines if the source and destination squares are in the
     *             same diagonal
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     True if the source and destination are in the same diagonal,
     *             else False.
     */
    bool sameDiag(int src, int dest) { return std::abs(src/8 - dest/8) == std::abs(src%8 - dest%8); }

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param[in]  dest   The destination square of pinning piece
     * @param      chess  The chess object is created
     *
     * @return     If only 1 piece is in the path from 'src' to 'dest', return
     *             its coordinates else, return -1 to indicate that there is
     *             either no piece or multiple pieces in the path.
     */
    int squareOfPieceInPath(int src, int dest, const Chess &chess)
    {
        int increment;
        vector<Piece*> board = chess.getBoard();
        vector<int> pieces_in_path;

        // determine the increment along the path from 'src' to 'dest' and store
        // all non empty squares in 'pieces_in_path'
        increment = incrementChoice(src, dest);
        if( (board[src]->isRook() && (increment == 1 || increment == 8)) ||
            (board[src]->isBishop() && (increment == 7 || increment == 9)) ||
            (board[src]->isQueen() && increment != 0) )
        {
            for(int i = std::min(src, dest)+increment; i < std::max(src, dest); i += increment)
                if(!board[i]->isEmpty())
                    pieces_in_path.push_back(i);
        }
        
        
        // only 1 piece ? return its square : return -1 (equivalent to false)
        return pieces_in_path.size() == 1 ? pieces_in_path[0] : -1;
    }

    /**
     * @brief      Used to determine the increment to use when moving a piece
     *             from 'src' to 'dest'.
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @return     Returns either 1 if moving in same row, 7 if moving in
     *             diagonal to the right, 8 if moving in same column, 9 if
     *             moving in diagonal to the left. If move does not correspond
     *             to one of the pieces, returns 0.
     */
    int incrementChoice(int src, int dest)
    {
        if(sameRow(src, dest))          // row path
            return 1;
        else if(sameCol(src, dest))     // column path
            return 8;
        else if(sameDiag(src, dest))    // diagonal path
            return std::abs(src - dest) % 7 == 0 ? 7 : 9;
        else
            return 0;
    }

    /**
     * @brief      Used to determine the coordinate of a pinned piece.
     *
     * @param[in]  src    The source square of pinning piece
     * @param      chess  The chess object is created
     * @param[in]  enemy  True if king color differs from piece color, else
     *                    false
     *
     * @return     Returns the coordinate position of the king, based on the
     *             current board representation and color determined by 'enemy'.
     */
    int findKingPos(int src, const Chess &chess, bool enemy)
    {
        Piece *temp;

        for(const auto & elem : chess.getBoard())
        {
            if( (enemy && elem->isKing() && !elem->isSameColor(src, chess) ) ||
                ( !enemy && elem->isKing() && elem->isSameColor(src, chess) ) )
            {
                temp = elem;
                break;
            }
        } 

        return temp->getPieceSquare();
    }

    /**
     * @brief      Gets the path to the object_states folder.
     *
     * @param[in]  num_moves  The number of moves made on the board
     *
     * @return     The path to the object_states folder.
     *
     * @note       The return value depends on target of the global makefile
     */
    // GCOVR_EXCL_START
    string getPath(int num_moves)
    {
        const unsigned long maxDir = 260;
        char currentDir[maxDir];
        GetCurrentDirectory(maxDir, currentDir);

        return string(currentDir).find("GUI") != string::npos ? "object_states/move" + to_string(num_moves) + ".txt" 
                                                              : "GUI/object_states/move" + to_string(num_moves) + ".txt"; 
    }
    // GCOVR_EXCL_STOP
} // unnamed namespace

/*************************************************************************************/
/*                              GLOBAL FUNCTIONS / OBJECTS                           */
/*************************************************************************************/
namespace chessCAMO
{
    /**
     * @brief      Iterates through the pieces on a current board representation
     *             to produce the board on the console screen
     *
     * @param      board      The board representation
     * @param      reservoir  The reservoir of pieces
     *
     * @post       Each piece of the current board representation is printed to
     *             the screen using a corresponding letter inside a formatted
     *             board. Additionally, the piece reservoir information is
     *             displayed beneath the board representation.
     */
    void printBoard(const vector<Piece*> &board, const vector<pair<int, char>> &reservoir)
    {
        char piece_char;
        char ranks[8] = {'8', '7', '6', '5', '4', '3', '2', '1'};
        
        int count = 0;
        for(const auto & elem : board)
        {
            if(count == 0)
                cout << "    A   B   C   D   E   F   G   H" << endl;

            if(count % 8 == 0)
                cout << "  +---+---+---+---+---+---+---+---+\n" << ranks[count/8] << " | ";

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

            if(!islower(piece_char))    // WHITE
                chessCAMO::printMessage(string(1, piece_char) + " ", GREEN);
            else                        // BLACK
                chessCAMO::printMessage(string(1, piece_char) + " ", RED);
            
            cout << "| ";

            // go to next row if reached last column
            if(count % 8 == 7)
                cout << ranks[count/8] << endl;
            if(count == 63)
                cout << "  +---+---+---+---+---+---+---+---+\n    A   B   C   D   E   F   G   H\n";
            count++;
        }

        // reservoir information
        chessCAMO::printMessage("           Piece Reservoir\n    ", CYAN);
        for(const auto & elem : reservoir)
        {
            if(std::tolower(elem.second) == 'o')
            {
                if((unsigned int) (&elem - &reservoir[0]) < reservoir.size()/2) { chessCAMO::printMessage("b", RED); }
                else { chessCAMO::printMessage("B", GREEN); }
            }
            else
            {
                if((unsigned int) (&elem - &reservoir[0]) < reservoir.size()/2) { chessCAMO::printMessage(string(1, elem.second), RED); }
                else { chessCAMO::printMessage(string(1, elem.second), GREEN); }
            }
            
            if((unsigned int) (&elem - &reservoir[0]) != reservoir.size()/2 - 1)
                cout << "  ";
            else
                cout << " | ";
        }
        cout << endl << "    ";

        for(const auto & elem : reservoir)
        {
            chessCAMO::printMessage(to_string(elem.first), DEFAULT);
            if((unsigned int) (&elem - &reservoir[0]) != reservoir.size()/2 - 1)
                cout << "  ";
            else
                cout << " | ";
        }
        cout << endl;
    }

    /**
     * @brief      Prints the footer message before each move indicating whose
     *             move it is for the current board representation.
     *
     * @param[in]  input_message  The input message that will be appended to the final message
     * @param      chess          The chess object
     */
    void printFooterMessage(string input_message, const Chess &chess)
    {
        cout << "___________________________________________________" << endl;
        string final_message = chess.getTurn() == WHITE ? "White" + input_message : "Black" + input_message;
        chessCAMO::printMessage(final_message, CYAN);
    }

    /**
     * @brief      At any moment, the players can either continue, draw, or
     *             resign
     *
     * @param[in]  clear_screen  Whether the screen should be cleared
     * @param      chess         The chess object is created
     * @param      in            Input stream is selected (stdin or file)
     *
     * @pre        None
     *
     * @post       Depending on the users choice, the program either continues
     *             ('y' || 'd' + 'n' || 'u') or terminates ('d' + 'y' || 'r')
     */
    void drawOrResign(bool clear_screen, Chess &chess, istream &in)
    {
        char user_input, draw_reply;
        string message;

        chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw, u -> undo move] ", PINK);
        in >> user_input;
        in.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)

        // error check
        while( std::tolower(user_input) != 'y' && std::tolower(user_input) != 'd' &&
               std::tolower(user_input) != 'r' && std::tolower(user_input) != 'u' )
        {
            chessCAMO::printMessage("Pick one of the choices... try again!", YELLOW);
            chessCAMO::printMessage("\nContinue? [y -> yes, r -> resign, d -> offer draw, u -> undo move] ", PINK);

            in >> user_input; // get new input
            in.ignore(100, '\n'); // ignore rest of the previous input (if invalid input was entered)
        }

        if(std::tolower(user_input) == 'r')
        {
            chessCAMO::clearScreen(clear_screen);
            chessCAMO::printBoard(chess.getBoard(), chess.getReservoir());
            message = chess.getTurn() == WHITE ? "\nWhite resigned => Black wins\n" 
                                               : "\nBlack resigned => White wins\n";
            chessCAMO::printMessage(message, CYAN);
            chess.setCheckmate(true); // to end the game
        }
        else if(std::tolower(user_input) == 'd')
        {
            chessCAMO::clearScreen(clear_screen);
            chessCAMO::printBoard(chess.getBoard(), chess.getReservoir());
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

            chessCAMO::clearScreen(clear_screen);
            chessCAMO::printBoard(chess.getBoard(), chess.getReservoir());

            if(std::tolower(draw_reply) == 'y')
            {
                chessCAMO::printMessage("\nGame drawn by agreement", CYAN);
                chess.setCheckmate(true); // to end the game
            }
            else // std::tolower(draw_reply) == 'n'
            {
                chessCAMO::printMessage("\nDraw rejected. Game continues...\n", CYAN);
                chessCAMO::printFooterMessage("'s move", chess);
            }
        }
        else if(std::tolower(user_input) == 'u')
        {
            // decrement move counter by 1 since an illegal move was made 
            chess.setNumMoves(chess.getNumMoves()-1);

            // restore previous object
            chessCAMO::restoreObject(chess.getNumMoves(), chess);

            // re-print board and display move information
            chessCAMO::clearScreen(clear_screen);
            chessCAMO::printBoard(chess.getBoard(), chess.getReservoir());
            chessCAMO::printFooterMessage("'s move", chess);
        }
        else { return ; } // do nothing, player wants to continue
    }

    /**
     * @brief      Clears the screen of the console window using a special
     *             string instead of a platform specific command
     *
     * @param[in]  apply  Whether to in fact clear or not
     */
    // GCOV_EXCL_START
    void clearScreen(bool apply) { cout << (apply ? "\033[2J\033[1;1H" : ""); }
    // GCOV_EXCL_STOP

    /**
     * @brief      Prints the given message ('text') with a given 'color' to
     *             console
     *
     * @param[in]  text   The text message to be created
     * @param[in]  color  One of the defined values at the top of the file
     *
     * @pre        None
     *
     * @post       The message is printed to the screen with color chosen and
     *             then the color is changed back to default prior to return
     */
    void printMessage(string text, int color)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
        cout << text;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
    }

    void saveObject(int num_moves, const Chess &chess_object)
    {
        string filename = getPath(num_moves);
        ofstream out(filename, ios::trunc);
        out << chess_object;
        out.close();
    }
    
    void restoreObject(int num_moves, Chess &chess_object)
    {
        // only undo if a move was made
        if(chess_object.getNumMoves() >= 0)
        {
            string filename = getPath(num_moves);
            ifstream in(filename);
            in >> chess_object;
            in.close(); 
        }
        // if undo was asked multiple times after 0, set number of moves to 0 and do nothing
        else { chess_object.setNumMoves(0); }
    }
}