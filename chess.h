/****************************************************************************************************/
/*                                  Title:           chess.h                                        */
/*                                  Author:          Lior Bragilevsky                               */
/*                                  Related Files:   chess.cpp                                      */
/*                                  Project:         chessCAMO                                      */
/*                                  Version:         1.0                                            */
/*                                  Last Revision:   June 15th, 2020                                */
/****************************************************************************************************/

/**
 * \todo
 * - <b> Regular Chess </b>:
 *   1. Three move repetition (draw) & 50 move rule.
 *   2. Board representation undo (use queue?).
 *   3. Input as PGN rather than two integers (template) - working currently but not really a template.
 *   4. GUI - nice to be able to move pieces with mouse rather than inputting coordinates.    
 * - <b> ChessCAMO </b>:
 *   1. Make piece reservoir (stack)
 *   2. Allow players to replace existing pieces with reservoir pieces
 *   3. Implement abovementioned check and pawn promotion rules
 *   4. Make test cases to confirm that overall functionality still works
 */

#ifndef CHESS_H // header guard to prevent multiple includes of the classes (results in compilation error)
#define CHESS_H

#include <iostream>
#include <iomanip>
#include <vector>
#include <stack>
#include <string>
#include <stdlib.h>     /* abs, system */
#include <algorithm>    /* min, max */
#include <fstream>
#include <windows.h> // for console text colors

using namespace std;

/*! \file */

/** Changes console text color to green */
#define GREEN 10

/** Changes console text color to cyan */
#define CYAN 11

/** Changes console text color to red */
#define RED 12

/** Changes console text color to pink */
#define PINK 13

/** Changes console text color to yellow */
#define YELLOW 14

/** Changes console text color to white (default) */
#define DEFAULT 15

/**
 * @brief      Piece's Type
 * 
 * \note
 * EMPTY corresponds to a square with no piece in it
 */
enum pieceType 
{
    PAWN,       ///< 0
    KNIGHT,     ///< 1
    BISHOP,     ///< 2
    ROOK,       ///< 3
    QUEEN,      ///< 4
    KING,       ///< 5
    EMPTY       ///< 6
};

/**
 * @brief      Piece's Color
 * 
 * \note
 * NEUTRAL corresponds to an empty square
 */
enum pieceColor 
{
    BLACK,      ///< 0
    NEUTRAL,    ///< 1
    WHITE       ///< 2
};

// forward declaration
class Piece; 

/*************************************************************************************/
/*                              CHESS CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
class Chess
{
public:
    /*********************************** BIG THREE *********************************/
    /**
     * @brief      Destructor - destroys the objects.
     */
    ~Chess();

    /**
     * @brief      Copy constructor - Constructs a new instance and copies the calling object's values to it.
     * 
     * @param[in]  object  The object whose values will be copied
     * 
     * \note Default
     */
    Chess(const Chess & object) = default; // copy constructor

    /**
     * @brief      Copy Assignment operator - assigns values of one object to another existing object
     *
     * @param[in]  object  The object whose values will be copied
     *
     * @return     The resulting object from the assignment
     * 
     * \note Default
     */
    Chess & operator =(const Chess & object) = default; // copy assignment
    /************************************* END *************************************/

    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             
     * \note
     * Intentionally left blank.
     */
    Chess() : checkmate{false}, stalemate{false}, check{false}, double_check{false}, turn{WHITE} {}

    /************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
    /**
     * @brief      (Accessor) Gets the board representation.
     *
     * @return     The board with current piece positions in correct indicies.
     */
    vector<Piece*> getBoard() const {return board;}

    /**
     * @brief      (Mutator) Sets the board representation.
     *
     * @param[in]  board  The current board representation
     */
    void setBoard(const vector<Piece*> & board) {this->board = board;}

    /**
     * @brief      (Accessor) Gets the check stack information.
     *
     * @return     The check stack.
     */
    stack<Piece*> getCheckStack() const {return checkStack;}

    /**
     * @brief      (Mutator) Sets the check stack information.
     *
     * @param[in]  checkStack  The check stack
     */
    void setCheckStack(const stack<Piece*> & checkStack) {this->checkStack = checkStack;}

    /**
     * @brief      (Accessor) Gets the check information.
     *
     * @return     The check.
     */
    bool getCheck() const {return check;}

    /**
     * @brief      (Mutator) Sets the check information.
     *
     * @param[in]  check  The check
     */
    void setCheck(bool check) {this->check = check;}

    /**
     * @brief      (Accessor) Gets the double check information.
     *
     * @return     The double check.
     */
    bool getDoubleCheck() const {return double_check;}

    /**
     * @brief      (Mutator) Sets the double check information.
     *
     * @param[in]  double_check  The double check
     */
    void setDoubleCheck(bool double_check) {this->double_check = double_check;}

    /**
     * @brief      (Accessor) Gets the checkmate information.
     *
     * @return     The checkmate.
     */
    bool getCheckmate() const {return checkmate;}

    /**
     * @brief      (Mutator) Sets the checkmate information.
     *
     * @param[in]  checkmate  The checkmate
     */
    void setCheckmate(bool checkmate) {this->checkmate = checkmate;}

    /**
     * @brief      (Accessor) Gets the stalemate information.
     *
     * @return     The stalemate.
     */
    bool getStalemate() const {return stalemate;}

    /**
     * @brief      (Mutator) Sets the stalemate information.
     *
     * @param[in]  stalemate  The stalemate
     */
    void setStalemate(bool stalemate) {this->stalemate = stalemate;}

    /**
     * @brief      (Accessor) Gets the player's turn information.
     *
     * @return     The turn.
     */
    pieceColor getTurn() const {return turn;}

    /**
     * @brief      (Mutator) Sets the player's turn information.
     *
     * @param[in]  turn  The turn
     */
    void setTurn(pieceColor turn) {this->turn = turn;} // useful when moving a piece
    /************************************* END *************************************/

    /**
     * @brief      Places the pieces on the board at their correct starting positions
     *
     * @param[in]  board_size  8x8 board has 64 index positions
     * 
     * \pre
     * The chess object is intialized
     * 
     * \post 
     * Instantiates new objects corresponding to the pieces, places them
     * in the corresponding index of the board vector and set the global
     * object's board variable
     */  
    void boardInit(int board_size = 64); // Board intialization

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
    void makeMove(int src, int dest, istream &in); 

    /** 
     * \brief 
     * Converts the input strings into their coordinates (integer)
     * and calls Chess::makeMove(int src, int dest, istream& in). 
     * 
     * \overload void Chess::makeMove(string,string,istream&)   
     */
    void makeMove(string src, string dest, istream &in);

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
    void isCheckmate(string check_type);
              
    /**
     * @brief      Decide if a move caused a stalemate

     * \pre
     * The chess object is created.
     * 
     * \post
     * Calls handleStalemate() to print messages to console indicating that game is drawn, if needed.
     * 
     * @return     True if board's state is in stalemate, else False and game continues as usual.
     */
    bool isStalemate();
    
private:
    /** Chess board representation with the pieces in correct spots */
    vector<Piece*> board;   

    /** Stores the pieces involved in a checking scenario */   
    stack<Piece*> checkStack; 

    /** Flag which ends the game and prints winner */
    bool checkmate;  

    /** Flag which ends the game and indicates that it is drawn */           
    bool stalemate; 

    /** Flag for single check detection */            
    bool check;    

    /** Flag for double check detection */             
    bool double_check;  

    /** Either WHITE or BLACK
     * \note NEUTRAL is not considered here
     */                                       
    pieceColor turn;            

	/*************************************************************************************/
	/*                              PIECE CLASS - HELPER FUNCTIONS                       */
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
    void makeMoveForType(int src, int dest);

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
    void pieceSwap(int src, int dest, vector<Piece*> & board);

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
    void handleChangeTurn();

    /**
     * @brief      Indicates which player won by checkmate via a message to console
     * 
     * \pre 
     * The chess object is created. A move was made (cannot checkmate in less than 2 moves in theory).
     * 
     * \post
     * Object's checkmate state is set to true (to end the algorithm)
     */
    void handleCheckmate();

    /**
     * @brief      Indicates the game is drawn via a message to console
     * \pre 
     * The chess object is created. A move was made.
     * 
     * \post
     * Object's stalemate state is set to true (to end the algorithm)
     */
    void handleStalemate();

    /**
     * @brief      After a move is made, can undo it if move was invalid and return to previous board state
     *
     * @param[in]  src            The source square of piece prior to current board state
     * @param[in]  dest           The destination square of piece prior to current board state
     * @param      king           The king that is being attacked currently
     * @param      piece          The piece that is attacking the king currently
     * @param      undo_piece     If move fails, this is the piece that was moved previously
     * @param[in]  undo_moveInfo  If move fails, this is the piece's move information
     * @param[in]  check_type     The check type (single or double)
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
    bool undoMove(int src, int dest, Piece* king, Piece* piece, Piece* undo_piece, bool undo_moveInfo, string check_type);
    
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
    bool singleCheckPieceIterator(Piece* piece, Piece* king); 
    
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
    bool doubleCheckPieceIterator(Piece* king);

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
    pieceColor switchTurn();
};

/*************************************************************************************/
/*                              PIECE CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
class Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             
     * \note
     * Intentionally left blank.
     */
    Piece() : square{0}, moved{false}, type{EMPTY}, color{NEUTRAL} {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Has 'moved' initialization.
     */
    Piece(int square, pieceType type, pieceColor color) 
        : moved{false}
    {
        this->square = square;
        this->type = type;
        this->color = color;
    }

    /************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
    /**
     * @brief      (Accessor) Gets the piece square information.
     *
     * @return     The piece square information.
     */
    int getPieceSquare() const {return square;}

    /**
     * @brief      (Mutator) Sets the piece square information.
     *
     * @param[in]  square  The piece square information
     */
    void setPieceSquare(int square) {this->square = square;}

    /**
     * @brief      (Accessor) Gets the piece type information.
     *
     * @return     The piece type information.
     */
    pieceType getPieceType() const {return type;}

    /**
     * @brief      (Mutator) Sets the piece type information.
     *
     * @param[in]  type  The piece type information
     */
    void setPieceType(pieceType type) {this->type = type;}

    /**
     * @brief      (Accessor) Gets the piece color information.
     *
     * @return     The piece color information.
     */
    pieceColor getPieceColor() const {return color;}

    /**
     * @brief      (Mutator) Sets the piece color information.
     *
     * @param[in]  color  The color information
     */
    void setPieceColor(pieceColor color) {this->color = color;}

    /**
     * @brief      (Accessor) Gets the piece move information useful for pawns, rooks, kings.
     *
     * @return     The piece move information.
     */
    bool getPieceMoveInfo() const {return moved;}

    /**
     * @brief      (Mutator) Sets the piece move information.
     *
     * @param[in]  moved  Indicates if piece moved
     */
    void setPieceMoveInfo(bool moved) {this->moved = moved;}    
    
    /**
     * @brief      (Accessor) Gets the piece en-passant ability information.
     *
     * @return     The piece en-passant ability information.
     */
    virtual bool getEnPassant() const {return false;}

    /**
     * @brief      (Mutator) Sets the piece en-passant ability information.
     *
     * @param[in]  en_passant  The piece en-passant ability information
     * 
     * \note
     * Intentionally left blank.
     */
    virtual void setEnPassant(bool en_passant) {}
    /************************************* END *************************************/

    /************************ TYPE DETERMINATION FUNCTIONS *************************/
    /**
     * @brief      Determines if the square is empty.
     *
     * @return     True if the square is empty, False otherwise.
     */
    bool isEmpty() {return this->getPieceType() == EMPTY;}

    /**
     * @brief      Determines if the piece is a pawn.
     *
     * @return     True if the piece is a pawn, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isPawn() {return this->getPieceType() == PAWN;}

    /**
     * @brief      Determines if the piece is a knight.
     *
     * @return     True if the piece is a knight, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isKnight() {return this->getPieceType() == KNIGHT;}

    /**
     * @brief      Determines if the piece is a bishop.
     *
     * @return     True if the piece is a bishop, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isBishop() {return this->getPieceType() == BISHOP;}

    /**
     * @brief      Determines if the piece is a rook.
     *
     * @return     True if the piece is a rook, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isRook() {return this->getPieceType() == ROOK;}

    /**
     * @brief      Determines if the piece is a queen.
     *
     * @return     True if the piece is a queen, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isQueen() {return this->getPieceType() == QUEEN;}

    /**
     * @brief      Determines if the piece is a king.
     *
     * @return     True if the piece is a king, False otherwise.
     * 
     * \note
     * Color of the piece is not considered
     */
    bool isKing() {return this->getPieceType() == KING;}
    /************************************* END *************************************/

    /************************ COLOR DETERMINATION FUNCTIONS ************************/
    /**
     * @brief      Determines if the piece is white.
     *
     * @return     True if the piece is white, False otherwise.
     */
    bool isPieceWhite() {return this->getPieceColor() == WHITE;}

    /**
     * @brief      Determines if the piece is black.
     *
     * @return     True if the piece is black, False otherwise.
     */
    bool isPieceBlack() {return this->getPieceColor() == BLACK;}
    /************************************* END *************************************/
    /**
     * @brief      Determines if 2 pieces have the same color
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if source piece color matches destination piece color, False otherwise.
     */
    bool isSameColor(int dest, Chess *chess);
    
    /**
     * @brief      Determines if a given piece is pinned to the king by opposing piece
     *
     * @param[in]  dest   The destination square of the piece  
     * @param      chess  The chess object
     *
     * @return     True if piece is pinned to the king and moving to 'dest' will cause
     *             the path (pinning piece -> king from pinned piece side) to be free, False otherwise.
     */
    bool isPinned(int dest, Chess *chess);

    /**
     * @brief      Determines if the path from the piece to its destination is empty
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if squares along the path (src, dest) are empty, False otherwise.
     */
    bool isPathFree(int dest, Chess *chess);

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
    bool isLegalMove(int dest, Chess *chess);

    /**
     * @brief      Did the move cause a check?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return    True if moving the piece to 'dest' now threatens the opposing king.
     *            False otherwise.
     */
    bool causeCheck(int dest, Chess *chess);

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
    bool causeDoubleCheck(int dest, Chess *chess);
    
    /**
     * @brief      Determine if the piece has a possible move towards the destination square
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if moving the piece to 'dest' is possible since the path is free, or 
     *             the piece is capable of making the move. False otherwise.
     */
    virtual bool isPossibleMove(int dest, Chess *chess) {return false;}

    /**
     * @brief      Pawn attacks opposing pawn with <a href="https://bit.ly/3cQj7G4" target="__blank">en-passant</a>
     *
     * @param[in]  src    The source square of the piece
     * @param      chess  The chess object
     * 
     * \pre
     * None
     * 
     * \post
     * En-passant private member is set to true if a pawn meets the criteria,
     * else all pawns have their en-passant abilities set to false. 
     */
    virtual void enPassantHandling(int src, Chess *chess) {return;}

    /**
     * @brief      Promotes the pawn if needed.
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     * @param      in     input stream type (stdin or file)
     * 
     * \pre
     * Pawn must be on a rank that can promote next move
     * 
     * \post
     * Changes the piece (pawn) to a stronger piece according to user input
     */
    virtual void promotePawn(int dest, Chess *chess, istream &in) {return;}

    /**
     * @brief      Can the king <a href="https://bit.ly/2XQEXFr" target="__blank">castle</a>?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if able to castle, False otherwise.
     */
    virtual bool canCastle(int dest, Chess *chess) {return false;}

    /**
     * @brief      Did the king move into check?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if a king moves into a square that another opposing piece
     *             also move into. False otherwise.
     */
    virtual bool movedIntoCheck(int dest, Chess *chess) {return false;}

private:
    /** position of the piece on the board [0, 63] -> [top left, bottom right] */
    int square;

    /** has the piece been moved yet? */       
    bool moved; 

    /** PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, OR EMPTY
     * \see pieceType
     */       
    pieceType type;

    /** BLACK, NEUTRAL, OR WHITE
     * \see pieceColor
     */     
    pieceColor color;   
};

/*************************************************************************************/
/*                              PAWN CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
class Pawn : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     *             
     * \note
     * Intentionally left blank. Has en-passant initialization.
     */
    Pawn() : Piece(), en_passant{false} {} // intentionally blank

    // constructor with valid piece information initialization
    
    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank. Has en-passant initialization.
     */
    Pawn(int square, pieceType type, pieceColor color) : Piece(square, type, color), en_passant{false} {}

    /************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
    // En-passant ability information 
    bool getEnPassant() const override {return en_passant;}
    void setEnPassant(bool en_passant) override {this->en_passant = en_passant;}
    /************************************* END *************************************/

    /**
      * \note
      * Can move 1 or 2 square (if not moved yet) forwards, attack diagonally 1 square,
      * en-passant, and promote.
      *   
      * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;

    /**
     * \see virtual Piece::enPassantHandling(int dest, Chess *chess)
     */
    void enPassantHandling(int src, Chess *chess) override;

    /**
     * \see virtual Piece::promotePawn(int dest, Chess *chess, istream &in)
     */
    void promotePawn(int dest, Chess *chess, istream &in) override;

private:
    /** * Can this pawn en-passant it's rival currently? */
    bool en_passant;
};  

/*************************************************************************************/
/*                              KNIGHT CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
class Knight : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    Knight() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    Knight(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Can move (2 up/down or 2 left/right) and (1 left/right or 1 up/down), can jump over pieces.
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;
};

/*************************************************************************************/
/*                              BISHOP CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
class Bishop : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    Bishop() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    Bishop(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Can move diagonally any number of squares.
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;
};

/*************************************************************************************/
/*                              ROOK CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
class Rook : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    Rook() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    Rook(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Can move horizontally or vertically any number of squares.
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;
};

/*************************************************************************************/
/*                              QUEEN CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
class Queen : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    Queen() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    Queen(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Combines rook and bishop moves.
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;
};

/*************************************************************************************/
/*                              KING CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
class King : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    King() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    King(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Combines rook and bishop moves but only 1 square
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess *chess)
     */
    bool isPossibleMove(int dest, Chess *chess) override;

    /**
     * \see virtual Piece::canCastle(int dest, Chess *chess)
     */
    bool canCastle(int dest, Chess *chess) override;

    /**
     * \see virtual Piece::movedIntoCheck(int dest, Chess *chess)
     */
    bool movedIntoCheck(int dest, Chess *chess) override;
};

/*************************************************************************************/
/*                              EMPTY CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
class Empty : public Piece
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             Calls Piece class constructor to intialize the member variables.
     *             
     * \note
     * Intentionally left blank.
     */
    Empty() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information initialization.
     *             Calls Piece class constructor to intialize the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     * 
     * \note
     * Intentionally left blank.
     */
    Empty(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}
};

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
    void printBoard(const vector<Piece*> & board); // Print the current board position

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
    void drawOrResign(Chess *chess, istream &in); // resign or draw

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
    void printMessage(string text, int color);
} // end namespace chessCAMO

#endif // CHESS_H