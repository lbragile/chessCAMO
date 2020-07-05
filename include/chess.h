 /**
  * \page chessheader Chess Header File (Functions & Todo List)
  * 
  * <b>Title</b><br>
  * <span>&emsp;&emsp;&emsp;chess.h </span>
  * \author Lior Bragilevsky<br>
  * 
  * <b>Related</b><br>
  * <span>&emsp;&emsp;&emsp;chess.cpp</span><br> 
  * 
  * <b>Project</b><br>
  * <span>&emsp;&emsp;&emsp;chessCAMO</span><br>
  * \version \version_num
  * \date \today
  * 
  * I decided to create this project due to my growing interest in software development.
  * Coming from an Engineering background with Deep Learning research experience
  * (Recent Master of Applied Science graduate), I realized that writing and developing code
  * (both front and back end) were highly appealing to me. Thus, I spent a lot of time building
  * upon my existing coding knowledge through online material and knew that it is time to test my
  * knowledge on a challenging project. Additionally, I played chess (not very seriously) consistently
  * since the age of about 4-5, so one can say that it is a very big part of my life. As a result,
  * this project allowed me to gain valuable experience and knowledge while working on a topic that
  * I am passionate about and have expertise in.
  * 
  * <b>YouTube - Look Out</b>
  * Be on the look out for a detailed YouTube video series explaining the various sections of this project.
  * I feel like others could benefit from the logic involved in creating a project on this scale, which is
  * relatively large and requires a substantial time commitment. Often times, when starting from scratch,
  * an inexperienced software developer will quickly get overwhelmed by the complexity and decide to end
  * the project before anything major is finished. I was in the same boat before and understand the
  * frustration of wanting to work on an interesting project, but simply not being able to. It is often
  * best to work on small pieces and build your way to the final product. This gives you a sense of
  * accomplishment at each step and prevents you from experiencing the overwhelmed feeling.
  * Let me know if you are interested in me making the videos and what you would like to see.
  * 
  * \todo
  * - <b>Regular Chess:</b>
  *   1. Three move repetition (draw) & 50 move rule.
  *   2. Board representation undo (use queue?).
  *   3. GUI - nice to be able to move pieces with mouse rather than inputting coordinates. 
  *   \n   
  * - <b>ChessCAMO:</b>
  *   1. Make piece reservoir (stack).
  *   2. Allow players to replace existing pieces with reservoir pieces.
  *   3. Implement abovementioned check and pawn promotion rules.
  *   4. Make test cases to confirm that overall functionality still works.
  */

#ifndef CHESS_H // header guard to prevent multiple includes of the classes (results in compilation error)
#define CHESS_H

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <windows.h>    // for console text colors

using namespace std;

/*! \file */  // to indicate that global variables are defined here (doxygen documentation)

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
 * @note       EMPTY corresponds to a square with no piece in it
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
 * @note       NEUTRAL corresponds to an empty square
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
/**
 * @brief      This class describes the chess board on which the game takes place. 
 *             It contains functions which analyze specific flags to determinewhen to
 *             switch turns, display warnings, and even end the game.
 */
class Chess
{
public:
    /**
     * @brief      Default constructor with default board parameter initialization - Constructs a new instance.
     *             
     * \note
     * Intentionally left blank.
     */
    Chess() : board(64), check_pieces(2), flags(4), turn{WHITE}, num_moves{0} {}

    /*********************************** BIG THREE *********************************/
    /**
     * @brief      Destroys the object and frees any dynamically allocated
     *             memory ('new') to avoid memory leaks.
     *
     * @note       This is needed since a game can end before a corresponding
     *             checkmate/stalemate flag is reached and thus 'board' and
     *             'check_pieces' will have dynamically allocated memory that
     *             isn't freed.
     */
    ~Chess();

    /**
     * @brief      Copy constructor - Constructs a new instance and copies the
     *             calling object's values to it.
     *
     * @param[in]  object  The object whose values will be copied
     *
     * @note       This is needed to be able to stack different board
     *             representations in the board positions stack. Otherwise,
     *             since the vector<Piece*> contains pointers, any updates to
     *             the position will be reflected in each layer.
     */
    Chess(const Chess &object) = default;

    /**
     * @brief      Copy Assignment operator - assigns values of one object to
     *             another existing object
     *
     * @param[in]  object  The object whose values will be copied
     *
     * @return     The resulting object from the assignment
     *
     * @note       Default
     */
    Chess & operator =(const Chess & object) = default;
    /************************************* END *************************************/

    /************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
    /**
     * @brief      (Accessor) Gets the board representation at the top of the
     *             board positions stack.
     *
     * @return     The board with current piece positions in correct indicies.
     */
    vector<Piece*> getBoard() const {return board;}

    /**
     * @brief      (Mutator) Updates the board representation at the top of the
     *             board positions stack.
     *
     * @param[in]  board  The current board representation
     */
    void setBoard(const vector<Piece*> & board) {this->board = board;}

    /**
     * @brief      (Accessor) Gets the check stack information.
     *
     * @return     The check stack after any given move.
     */
    vector<Piece*> getCheckPieces() const {return check_pieces;}

    /**
     * @brief      (Mutator) Sets the check stack information.
     *
     * @param[in]  check_pieces  The check stack which contains the board
     *                           representations after each move
     */
    void setCheckPieces(vector<Piece*> check_pieces) {this->check_pieces = check_pieces;}

    /**
     * @brief      (Accessor) Gets the check information.
     *
     * @return     True if board representation has a check, False otherwise.
     */
    bool getCheck() const {return flags[0];}

    /**
     * @brief      (Mutator) Sets the check information.
     *
     * @param[in]  check  The check flag
     */
    void setCheck(bool check) {flags[0] = check;}

    /**
     * @brief      (Accessor) Gets the double check information.
     *
     * @return     True if board representation has a double check, False otherwise.
     */
    bool getDoubleCheck() const {return flags[1];}

    /**
     * @brief      (Mutator) Sets the double check information.
     *
     * @param[in]  double_check  The double check flag
     */
    void setDoubleCheck(bool double_check) {flags[1] = double_check;}

    /**
     * @brief      (Accessor) Gets the checkmate information.
     *
     * @return     True if board representation has a checkmate, False otherwise.
     */
    bool getCheckmate() const {return flags[2];}

    /**
     * @brief      (Mutator) Sets the checkmate information.
     *
     * @param[in]  checkmate  The checkmate flag
     */
    void setCheckmate(bool checkmate) {flags[2] = checkmate;}

    /**
     * @brief      (Accessor) Gets the stalemate information.
     *
     * @return     True if board representation has a stalemate, False otherwise.
     */
    bool getStalemate() const {return flags[3];}

    /**
     * @brief      (Mutator) Sets the stalemate information.
     *
     * @param[in]  stalemate  The stalemate flag
     */
    void setStalemate(bool stalemate) {flags[3] = stalemate;}

    /**
     * @brief      (Accessor) Gets the player's turn information.
     *
     * @return     The turn at any given moment (either white's or black's).
     */
    pieceColor getTurn() const {return turn;}

    /**
     * @brief      (Mutator) Sets the player's turn information.
     *
     * @param[in]  turn  The turn
     */
    void setTurn(pieceColor turn) {this->turn = turn;}

    /**
     * @brief      (Accessor) Gets the number of moves made.
     *
     * @return     The number of moves.
     */
    int getNumMoves() { return num_moves;}

    /**
     * @brief      (Mutator) Sets the number of moves made on the board.
     *
     * @param[in]  num_moves  The number of moves made
     */
    void setNumMoves(int num_moves) {this->num_moves = num_moves;}
    /************************************* END *************************************/

    /**
     * @brief      Places the pieces on the board at their correct starting
     *             positions
     *
     * @pre        The chess object is intialized
     *
     * @post       Instantiates new objects corresponding to the pieces, places
     *             them in the corresponding index of the board vector and set
     *             the global object's board variable
     */  
    void boardInit();

    /**
     * @brief      Moves a piece on the board from 'src' to 'dest' if conditions
     *             for a legal move are met.
     *
     * @param[in]  src   The source square (piece's current location)
     * @param[in]  dest  The destination square (piece's ending location)
     * @param      in    input stream type (stdin or file)
     *
     * @pre        The chess object is created.
     *
     * @post       The pieces at 'src' and 'dest' positions are swapped. If
     *             needed (attacking, castling, etc.) an empty square is made.
     *             The board's state is updated to indicate that the move
     *             occured. On failure, an error message is printed and user is
     *             asked to retry.
     *
     * @return     True if move was made, False otherwise.
     */
    bool makeMove(int src, int dest, istream &in); 

    /**
     * @brief      Decide if a move caused a checkmate according to 'check_type'
     *
     * @param[in]  check_type  Either "single" or "double" corresponding to the
     *                         check on the board
     *
     * @pre        The chess object is created.
     *
     * @post       If board's state is in checkmate, calls
     *             Chess::handleCheckmate() to print messages to console
     *             indicating the winner. Else, game continues as usual.
     */
    void isCheckmate(string check_type);
              
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
    bool isStalemate();
    
    /**
	 * @brief      Overloaded extraction operator
	 *
	 * @param      out           The output type (ex. ofstream or cout)
	 * @param[in]  chess_object  The chess object
	 *
	 * @return     The output stream type
	 */
    friend ostream & operator << (ostream &out, const Chess &chess_object);

    /**
	 * @brief      Overlaoded insertion operator
	 *
	 * @param      in            The input type (ex. ifstream or cin)
	 * @param      chess_object  The chess object
	 *
	 * @return     The input stream type
	 */
    friend istream & operator >> (istream &in, Chess &chess_object);

private:
	/** The current board representation of the pieces in the correct positions */
    vector<Piece*> board;

	/** The pieces involved in a check */
    vector<Piece*> check_pieces;

	/** Check, Double Check, Checkmate, Stalemate flags */
    vector<bool> flags;

	/** For deciding whose turn it is to make a move for a given board representation */
    pieceColor turn;

	/** The number of moves made already */
    int num_moves;          

	/*************************************************************************************/
	/*                              PIECE CLASS - HELPER FUNCTIONS                       */
	/*************************************************************************************/
    /**
     * @brief      A move can be one of: attack, castle, en-passant, regular
     *
     * @param[in]  src   The source square of piece
     * @param[in]  dest  The destination square of piece
     *
     * @pre        The chess object is created
     *
     * @post       Swaps the pieces on the board according to 'src' and 'dest'
     *             and proper chess rules, using Chess::pieceSwap(int src, int
     *             dest, vector<Piece*> & board). If a new empty square must be
     *             created, this is handled. Returns board representation with
     *             the made move.
     */
    void makeMoveForType(int src, int dest);

    /**
     * @brief      Used in Chess::makeMoveForType(int src, int dest) to swap
     *             pieces on the board
     *
     * @param[in]  src    The source square of piece
     * @param[in]  dest   The destination square of piece
     * @param      board  The current board representation
     *
     * @pre        The chess object is created
     *
     * @post       Swaps the pieces on the board according to 'src' and 'dest'.
     */
    void pieceSwap(int src, int dest, vector<Piece*> & board);

    /**
     * @brief      Indicates who will move next via a message to console
     *
     * @pre        The chess object is created
     *
     * @post       Player turn is switched, board is printed, and message is
     *             displayed if game is not over to indicate whose turn it is.
     */
    void handleChangeTurn();

    /**
     * @brief      Indicates which player won by checkmate via a message to
     *             console
     *
     * @pre        The chess object is created. A move was made (cannot
     *             checkmate in less than 2 moves in theory).
     *
     * @post       Object's checkmate state is set to true (to end the
     *             algorithm)
     */
    void handleCheckmate();

    /**
     * @brief      Indicates the game is drawn via a message to console
     * 
     * @pre        The chess object is created. A move was made.
     *
     * @post       Object's stalemate state is set to true (to end the
     *             algorithm)
     */
    void handleStalemate();

    /**
     * @brief      After a move is made, can undo it if move was invalid and
     *             return to previous board state
     *
     * @param      piece  The piece that is attacking the king currently
     *
     * @pre        The chess object is created. A move was made.
     *
     * @post       None
     *
     * @return     True if after move, the 'king' is still in check (single or
     *             double) or the move was invalid, output warning message and
     *             undo the move. Else, False and continue the game without
     *             undoing the move.
     */
    bool needUndoMove(Piece *piece);

    /**
     * @brief      If in a single check, see if piece can defend the king,
     *             capture attacking piece, or move the king out of check. Used
     *             in isCheckmate("single")
     *
     * @param      piece  The piece that is attacking the king currently
     * @param      king   The king that is being attacked currently
     *
     * @pre        The chess object is created. A move was made.
     *
     * @post       None
     *
     * @return     True if no legal moves found (checkmate), else False and make
     *             the move
     */
    bool singleCheckPieceIterator(Piece *piece, Piece *king); 
    
    /**
     * @brief      If in a double check, see if the king can move out of check
     *             as this is the only valid move option. Used in
     *             isCheckmate("double").
     *
     * @param      king  The king that is being attacked currently
     *
     * @pre        The chess object is created.
     *
     * @post       None
     *
     * @return     True if no legal moves found (checkmate), else False and make
     *             the move
     */
    bool doubleCheckPieceIterator(Piece *king);

    /**
     * @brief      Decides whose turn it is currently and updates the private
     *             member variable ('turn') accordingly
     *
     * @pre        The chess object is created.
     *
     * @post       None
     *
     * @return     'chess.turn' is set to the correct player
     */
    pieceColor switchTurn();
};

/*************************************************************************************/
/*                              PIECE CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      This class describes the pieces on the board at any given moment.
 *             It contains functions for determining the piece's type, color,
 *             possible/legal moves, and more helpful utilities for making
 *             decisions regarding move candidates.
 */
class Piece
{
public:
	virtual ~Piece() {}
	/**
	 * @brief      Default constructor with default board parameter
	 *             initialization - Constructs a new instance.
	 *
	 * @note       Intentionally left blank.
	 */
    Piece() : square{0}, moved{false}, type{EMPTY}, color{NEUTRAL} {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Has 'moved' initialization.
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
     * @brief      (Accessor) Gets the piece en-passant ability information for left side.
     *
     * @return     The piece en-passant ability information in left direction.
     */
    virtual bool getEnPassantLeft() const {return false;}

    /**
     * @brief      (Mutator) Sets the piece en-passant ability information for
     *             left side.
     *
     * @param[in]  en_passant_left  The piece en-passant ability information in
     *                              left direction
     *
     * @note       Intentionally left blank.
     */
    virtual void setEnPassantLeft(bool en_passant_left) {}

    /**
     * @brief      (Accessor) Gets the piece en-passant ability information for
     *             right side.
     *
     * @return     The piece en-passant ability information in right direction.
     */
    virtual bool getEnPassantRight() const {return false;}

    /**
     * @brief      (Mutator) Sets the piece en-passant ability information  for
     *             right side.
     *
     * @param[in]  en_passant_right  The piece en-passant ability information in
     *                               right direction
     *
     * @note       Intentionally left blank.
     */
    virtual void setEnPassantRight(bool en_passant_right) {}
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
     * @return     True if source piece color matches destination piece color,
     *             False otherwise.
     */
    bool isSameColor(int dest, Chess &chess);
    
    /**
     * @brief      Determines if a given piece is pinned to the king by opposing
     *             piece
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if piece is pinned to the king and moving to 'dest' will
     *             cause the path (pinning piece -> king from pinned piece side)
     *             to be free, False otherwise.
     */
    bool isPinned(int dest, Chess &chess);

    /**
     * @brief      Determines if the path from the piece to its destination is
     *             empty
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if squares along the path (src, dest) are empty, False
     *             otherwise.
     */
    bool isPathFree(int dest, Chess &chess);

    /**
     * @brief      Determines if a move is legal based on the rules of chess
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if moving the piece to 'dest' is legal from any type of
     *             move and piece, False otherwise.
     *
     * @note       A possible move, is not necessarily legal.
     */
    bool isLegalMove(int dest, Chess &chess);

    /**
     * @brief      Did the move cause a check?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if moving the piece to 'dest' now threatens the opposing
     *             king. False otherwise.
     */
    bool causeCheck(int dest, Chess &chess);

    /**
     * @brief      Did the move cause a double check?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if moving the piece to 'dest' now threatens the opposing
     *             king, and an additional piece from the same side also has a
     *             legal move towards the opposing king. False otherwise.
     */
    bool causeDoubleCheck(int dest, Chess &chess);
    
    /**
     * @brief      Determine if the piece has a possible move towards the
     *             destination square
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if moving the piece to 'dest' is possible since the path
     *             is free, or the piece is capable of making the move. False
     *             otherwise.
     */
    virtual bool isPossibleMove(int dest, Chess &chess) {return false;}

    /**
     * @brief      Pawn attacks opposing pawn with <a
     *             href="https://bit.ly/3cQj7G4" target="__blank">en-passant</a>
     *
     * @param[in]  src    The source square of the piece
     * @param      chess  The chess object
     *
     * @pre        None
     *
     * @post       En-passant private member is set to true if a pawn meets the
     *             criteria, else all pawns have their en-passant abilities set
     *             to false.
     */
    virtual void enPassantHandling(int src, Chess &chess) {return;}

    /**
     * @brief      Promotes the pawn if needed.
     *
     * @param      chess  The chess object
     * @param      in     input stream type (stdin or file)
     *
     * @pre        Pawn must be on a rank that can promote next move
     *
     * @post       Changes the piece (pawn) to a stronger piece according to
     *             user input
     */
    virtual void promotePawn(Chess &chess, istream &in) {return;}

    /**
     * @brief      Can the king <a href="https://bit.ly/2XQEXFr"
     *             target="__blank">castle</a>?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if able to castle, False otherwise.
     */
    virtual bool canCastle(int dest, Chess &chess) {return false;}

    /**
     * @brief      Did the king move into check?
     *
     * @param[in]  dest   The destination square of the piece
     * @param      chess  The chess object
     *
     * @return     True if a king moves into a square that another opposing
     *             piece also move into. False otherwise.
     */
    virtual bool movedIntoCheck(int dest, Chess &chess) {return false;}

private:
    /** position of the piece on the board [0, 63] -> [top left, bottom right] */
    int square;

    /** has the piece been moved yet? */       
    bool moved; 

    /** PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, OR EMPTY
     * @see        pieceType
     */       
    pieceType type;

    /** BLACK, NEUTRAL, OR WHITE
     * @see        pieceColor
     */     
    pieceColor color;   
};

/*************************************************************************************/
/*                              PAWN CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * @brief      This class describes a pawn and provides functions related to
 *             pawn movements.
 */
class Pawn : public Piece
{
public:
	~Pawn() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  *
	  * @note       Intentionally left blank. Has en-passant initialization.
	  */
    Pawn() : Piece(), en_passant_left{false}, en_passant_right{false} {}

    // constructor with valid piece information initialization
    
    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank. Has en-passant initialization.
     */
    Pawn(int square, pieceType type, pieceColor color)
     : Piece(square, type, color), en_passant_left{false}, en_passant_right{false} {}

    /************************ MUTATOR & ACCESSOR FUNCTIONS ************************/
    // En-passant ability information (LEFT)
    bool getEnPassantLeft() const override {return en_passant_left;}
    void setEnPassantLeft(bool en_passant_left) override {this->en_passant_left = en_passant_left;}

    // En-passant ability information (RIGHT)
    bool getEnPassantRight() const override {return en_passant_right;}
    void setEnPassantRight(bool en_passant_right) override {this->en_passant_right = en_passant_right;}
    /************************************* END *************************************/

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
    bool isPossibleMove(int dest, Chess &chess) override;

    /**
     * @see        virtual Piece::enPassantHandling(int dest, Chess &chess)
     *
     * @brief      Captures the violating pawn if an en-passant move it made.
     *
     * @param[in]  src    The piece's source square
     * @param      chess  The chess object
     */
    void enPassantHandling(int src, Chess &chess) override;

    /**
     * @see        virtual Piece::promotePawn(int dest, Chess &chess, istream
     *             &in)
     *
     * @brief      Promotes the pawn to a piece depending on the user's choice.
     *
     * @param      chess  The chess object
     * @param      in     The input stream type (ex. ifstream or cin)
     */
    void promotePawn(Chess &chess, istream &in) override;

private:
    /** * Can this pawn en-passant it's left rival currently? */
    bool en_passant_left;

    /** * Can this pawn en-passant it's right rival currently? */
    bool en_passant_right;
};  

/*************************************************************************************/
/*                              KNIGHT CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
/**
 * @brief      This class describes a knight and provides functions related to
 *             knight movements.
 */
class Knight : public Piece
{
public:
	~Knight() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    Knight() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    Knight(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

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
    bool isPossibleMove(int dest, Chess &chess) override;
};

/*************************************************************************************/
/*                              BISHOP CLASS - MEMBER FUNCTIONS                      */
/*************************************************************************************/
/**
 * @brief      This class describes a bishop and provides functions related to
 *             bishop movements.
 */
class Bishop : public Piece
{
public:
	~Bishop() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    Bishop() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    Bishop(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

    /**
     * \note
     * Can move diagonally any number of squares.
     * 
     * \see virtual Piece::isPossibleMove(int dest, Chess &chess)
     */
    bool isPossibleMove(int dest, Chess &chess) override;
};

/*************************************************************************************/
/*                              ROOK CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * @brief      This class describes a rook and provides functions related to rook movements.
 */
class Rook : public Piece
{
public:
	~Rook() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    Rook() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    Rook(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

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
    bool isPossibleMove(int dest, Chess &chess) override;
};

/*************************************************************************************/
/*                              QUEEN CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      This class describes a queen and provides functions related to queen movements.
 */
class Queen : public Piece
{
public:
	~Queen() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    Queen() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    Queen(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

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
    bool isPossibleMove(int dest, Chess &chess) override;
};

/*************************************************************************************/
/*                              KING CLASS - MEMBER FUNCTIONS                        */
/*************************************************************************************/
/**
 * @brief      This class describes a king and provides functions related to
 *             king movements.
 */
class King : public Piece
{
public:
	~King() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    King() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    King(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}

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
    bool isPossibleMove(int dest, Chess &chess) override;

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
    bool canCastle(int dest, Chess &chess) override;

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
    bool movedIntoCheck(int dest, Chess &chess) override;
};

/*************************************************************************************/
/*                              EMPTY CLASS - MEMBER FUNCTIONS                       */
/*************************************************************************************/
/**
 * @brief      This class describes an empty square which is necessary to keep
 *             in mind when playing a game of chess.
 */
class Empty : public Piece
{
public:
	~Empty() {}

	/**
	  * @brief      Default constructor with default board parameter
	  *             initialization - Constructs a new instance. Calls Piece
	  *             class constructor to intialize the member variables.
	  *
	  * @note       Intentionally left blank.
	  */
    Empty() : Piece() {}

    /**
     * @brief      Constructs a new instance with valid piece information
     *             initialization. Calls Piece class constructor to intialize
     *             the member variables.
     *
     * @param[in]  square  The square of the piece
     * @param[in]  type    The type of the piece
     * @param[in]  color   The color of the piece
     *
     * @note       Intentionally left blank.
     */
    Empty(int square, pieceType type, pieceColor color) : Piece(square, type, color) {}
};

/*************************************************************************************/
/*                              GLOBAL FUNCTIONS / OBJECTS                           */
/*************************************************************************************/
/**
 * @brief      This namespace contains the global functions related to chessCAMO
 *             which are mainly used as helper functions to print information to
 *             the console to improve user experience while playing the game.
 */
namespace chessCAMO
{
    /**
     * @brief      Converts the input string into its corresponding coordinate
     *             (integer) for easier computation in chess::makeMove(int src,
     *             int dest, istream& in).
     *
     * @param[in]  input  The input square of the piece (as a string, E.g. "e2")
     *
     * @tparam     T      Either string or integer type
     *
     * @return     The source or destination squares in integer form
     *
     * @note       Can use integers as well due to template functionality
     */
    template<class T>
    int preProcessInput(T input)
    {
        if(std::isalpha(input[0]))
        {
            int ascii_val = std::islower(input[0]) ? 97 : 65;
            return (int(input[0]) - ascii_val) + (8 - (int(input[1]) - 48))*8; // file + rank
        }
        
        return std::stoi(input);
    }

    /**
     * @brief      Iterates through the pieces on a current board representation
     *             to produce the board on the console screen
     *
     * @param[in]  board  The board representation
     *
     * @post       Each piece of the current board representation is printed to
     *             the screen using a corresponding letter inside a formatted
     *             board
     */
    void printBoard(const vector<Piece*> board); // Print the current board position

    /**
     * @brief      Prints the footer message before each move indicating whose
     *             move it is for the current board representation.
     *
     * @param      chess  The chess object
     */
    void printFooterMessage(const Chess &chess);

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
    void drawOrResign(bool clear_screen, Chess &chess, istream &in);

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
    void printMessage(string text, int color);

    /**
     * @brief      Clears the screen of the console window using a special
     *             string instead of a platform specific command
     *
     * @param[in]  apply  Whether to in fact clear or not
     */
    void clearScreen(bool apply);

    /**
     * @brief      Saves an object by serializing its member fields to a text
     *             file, allowing it to later be reset.
     *
     * @param[in]  num_moves     The number of moves made
     * @param[in]  chess_object  The chess object
     */
    void saveObject(int num_moves, const Chess & chess_object);

    /**
     * @brief      De-serializes an object from a file based on the number of
     *             moves made, essentially restoring the object's saved
     *             properties (in the file).
     *
     * @param[in]  num_moves     The number of moves made
     * @param      chess_object  The chess object
     */
    void restoreObject(int num_moves, Chess & chess_object);
} // end namespace chessCAMO

#endif // CHESS_H

// main page of doxygen documentation file

/**
 * @mainpage chessCAMO
 * ## Chess engine with a twist in the form of a new variant
 * # Project Description
 * <hr style="border: 1px solid #EAECEF;">
 * *chessCAMO* is a newly proposed chess engine with a built-in variant that stands for <b>C</b>alculations <b>A</b>lways <b>M</b>ake <b>O</b>pportunities
 * and was inspired by the very popular chess variant <a href="https://en.wikipedia.org/wiki/Crazyhouse" target="__blank" style="text-decoration: none;">Crazyhouse</a>.
 * 
 * The "CAMO" portion comes from the idea that in this variant, at any turn, a player can sacrifice making a move to replace one of their pieces with a piece from the "piece reservoir".
 * This has the affect of camouflaging each piece that is currently on the board. 
 * 
 * [![Build](https://img.shields.io/github/workflow/status/lbragile/chessCAMO/Unit_Testing/master)](https://github.com/lbragile/chessCAMO/actions)
 * [![Coverage](https://img.shields.io/codecov/c/github/lbragile/chessCAMO/master)](https://codecov.io/gh/lbragile/chessCAMO/commits)
 * [![Issues](https://img.shields.io/github/issues/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/issues)
 * [![Forks](https://img.shields.io/github/forks/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO)
 * [![Stars](https://img.shields.io/github/stars/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO)
 * [![Documentation](https://img.shields.io/badge/doc-available-brightgreen.svg)](https://lbragile.github.io/chessCAMO/)
 * [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/lbragile/chessCAMO)
 * [![Active Development](https://img.shields.io/badge/maintenance-active-brightgreen.svg)](https://github.com/lbragile/chessCAMO)
 * [![License](https://img.shields.io/github/license/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/blob/master/LICENSE)
 * 
 * <img src="gui.gif" alt="checkmate" style="float: left; zoom:100%;" />
 * \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n
 * 
 * <img src="gameplay.gif" alt="checkmate" style="float: left; zoom:100%;" />
 * \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n
 * 
 * # Installation
 * <hr style="border: 1px solid #EAECEF;">
 * ## Graphical User Interface (GUI)
 * ### Compiler
 * - Can be used **without** any pre-requests besides the files found in this repository
 * 
 * ### Clone
 * - <span style="background-color:#F3F4F4;">git clone https://github.com/lbragile/chessCAMO.git</span>
 * 
 * ### Run
 * - <span style="background-color:#F3F4F4;">cd chessCAMO/GUI/</span>
 * - <span style="background-color:#F3F4F4;">chessCAMO</span>
 * 
 * ## Console (Windows)
 * ### Compiler
 * - A GCC/G++ <a href="https://www.ics.uci.edu/~pattis/common/handouts/mingweclipse/mingw.html" target="__blank" style="text-decoration: none;">compiler</a> is required!
 * 
 * ### Clone
 * - <span style="background-color:#F3F4F4;">git clone https://github.com/lbragile/chessCAMO.git</span>
 * 
 * ### Run
 * - <span style="background-color:#F3F4F4;">cd chessCAMO</span>
 * - <span style="background-color:#F3F4F4;">mingw32-make all_main</span>
 * - <span style="background-color:#F3F4F4;">main</span>
 *
 * # Variant's Rules
 * <hr style="border: 1px solid #EAECEF;">
 * 1. The piece reservoir is limited in size and cannot be re-stocked with pieces.
 * 2. Once a player decides to use a piece from the reservoir to replace one of his existing pieces on the board, their turn is over.
 * 3. An obvious rule is that player cannot use the reservoir while in check as they must make a move to avoid check.
 * 4. Additionally, just like in the *Crazyhouse* variant, a player cannot directly promote a pawn from the reservoir.
 * That is a pawn from the reservoir must be placed on the board first and is only allowed to be promoted on the next move.
 * 
 * # Documentation
 * <hr style="border: 1px solid #EAECEF;">
 * [![Documentation](https://img.shields.io/badge/doc-available-brightgreen.svg)](https://lbragile.github.io/chessCAMO/)
 * 
 * Check out the very detailed <a href="https://lbragile.github.io/chessCAMO/" target="__blank" style="text-decoration: none;">documentation file</a> to clear up any questions you might have.
 * 
 * # To Do
 * <hr style="border: 1px solid #EAECEF;">
 * ### Regular Chess: 
 * - [ ] Three move repetition (draw) & 50 move rule.
 * - [x] Board representation undo <strike>(use queue?)</strike> - used serialization.
 * - [x] Input as PGN rather than two integers (template).
 * - [x] GUI - <strike>nice to be able to move pieces with mouse rather than inputting coordinates.</strike> in progress (more details for user in the interface will be added).
 * 
 * ### ChessCAMO:
 * - [ ] Make piece reservoir (stack).
 * - [ ] Allow players to replace existing pieces with reservoir pieces.
 * - [ ] Implement abovementioned check and pawn promotion rules.
 * - [ ] Make test cases to confirm that overall functionality still works.
 * 
 * # Tests
 * <hr style="border: 1px solid #EAECEF;">
 * [![Build](https://img.shields.io/github/workflow/status/lbragile/chessCAMO/Unit_Testing/master)](https://github.com/lbragile/chessCAMO/actions)
 * [![Coverage](https://img.shields.io/codecov/c/github/lbragile/chessCAMO/master)](https://codecov.io/gh/lbragile/chessCAMO/commits)
 * 
 * <img src="testing.gif" alt="testing" align="left" style="zoom:100%;" />
 * 
 * \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n
 * 
 * ## Run
 * - <span style="background-color:#F3F4F4;">mingw32-make all_unit</span>
 * - <span style="background-color:#F3F4F4;">unit --gtest_color=yes</span>
 *   <span>&larr;</span> colored output (<span style="color:green">Pass</span>/<span style="color:red">Fail</span>) 
 * <br><b> OR </b><br>
 * - <span style="background-color:#F3F4F4;">mingw32-make all_unit</span>
 * - <span style="background-color:#F3F4F4;">unit</span> <span>&larr;</span> regular output (no color) 
 * 
 * ## Unit Testing
 * Currently chessCAMO utilizes the <a href="https://github.com/google/googletest" target="__blank" style="text-decoration: none;">GoogleTest</a> Framework
 * to create test fixtures that cover as much of the implementation file as possible.
 * 
 * You can contribute to this project by making test cases that can challenge the chessCAMO engine in unique ways to ensure that it follows proper chess rules.
 * To do so, please refer to the appropriate <a href="https://lbragile.github.io/chessCAMO/unit.html" target="__blank" style="text-decoration: none;">section</a> in the documentation file.
 * 
 * # Contributing 
 * <hr style="border: 1px solid #EAECEF;">
 * [![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](https://github.com/lbragile/chessCAMO)
 * [![Issues](https://img.shields.io/github/issues/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/issues)
 * [![Forks](https://img.shields.io/github/forks/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO) 
 * \n\n
 * Please refer to the <a href="https://github.com/lbragile/chessCAMO/blob/master/CONTRIBUTING.md" target="__blank" style="text-decoration: none;">CONTRIBUTING.md</a> document.
 * 
 * # Like What You See?
 * [![Stars](https://img.shields.io/github/stars/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO)
 * ## Give Stars
 * Make the maintainer happy by hitting the star icon for this repository!
 * \n
 * <img src="star.PNG" alt="star this repo" style="float: left; zoom:100%;" />
 * \n\n\n\n\n
 * 
 * ## Add to Watch List
 * Consider adding this repository to your watch list to stay updated on what new feature is implements.
 * \n
 * <img src="watch.PNG" alt="watch list addition" style="float: left; zoom:100%;" />
 * \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n
 *
 * # Inspiration​
 * <hr style="border: 1px solid #EAECEF;">
 * I decided to create this project due to my growing interest in software development. 
 * Coming from an Engineering background with Deep Learning research experience (Recent Master of Applied Science graduate), 
 * I realized that writing and developing code (both front and back end) were highly appealing to me. 
 * Thus, I spent a lot of time building upon my existing coding knowledge through online material and knew that it is time to test my knowledge on a challenging project. 
 * Additionally, I played chess (not very seriously) consistently since the age of about 4-5, so one can say that it is a very big part of my life.
 * As a result, this project allowed me to gain valuable experience and knowledge while working on a topic that I am passionate about and have expertise in.
 * 
 * # YouTube - Look Out
 * <hr style="border: 1px solid #EAECEF;">
 * Be on the look out for a detailed YouTube video series explaining the various sections of this project.
 * I feel like others could benefit from the logic involved in creating a project on this scale, which is relatively large and requires a substantial time commitment.
 * Often times, when starting from scratch, an inexperienced software developer will quickly get overwhelmed by the complexity and decide to end the project before anything major is finished.
 * I was in the same boat before and understand the frustration of wanting to work on an interesting project, but simply not being able to. 
 * It is often best to work on small pieces and build your way to the final product. 
 * This gives you a sense of accomplishment at each step and prevents you from experiencing the overwhelmed feeling. 
 * Let me know if you are interested in me making the videos and what you would like to see.
 * 
 * # Need Support?
 * <hr style="border: 1px solid #EAECEF;">
 * Reach out to me at any of the following:
 * - <a href="https://www.linkedin.com/in/liorbragilevsky/" target="__blank" style="text-decoration: none;">LinkedIn</a>
 * - <a href="https://www.facebook.com/lbragi/" target="__blank" style="text-decoration: none;">Facebook</a>
 * - lbragile.masc@gmail.com
 * 
 * # License
 * <hr style="border: 1px solid #EAECEF;">
 * [![License](https://img.shields.io/github/license/lbragile/chessCAMO)](https://github.com/lbragile/chessCAMO/blob/master/LICENSE)
 * - <a href="https://github.com/lbragile/chessCAMO/blob/master/LICENSE" target="__blank" style="text-decoration: none;"><b>MIT License</b></a> 
 * - C​o​p​y​ri​gh​t​ (c) 2020 Lior Bragilevsky
 */
