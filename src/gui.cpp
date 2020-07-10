/**
 * @page gui GUI Library File Description (Allow User to Play Via a User Interface)
 *
 * <b>Title</b><br> <span>&emsp;&emsp;&emsp;main.cpp</span>
 * @author     Lior Bragilevsky<br>
 *
 * <b>Related</b><br> <span>&emsp;&emsp;&emsp;chess.h</span><br>
 *
 * <b>Project</b><br> <span>&emsp;&emsp;&emsp;chessCAMO</span><br>
 * @version    \version_num
 * @date       \today
 *
 * This library file complements the main implementation file by providing a
 * slick User Interface. Once the program is running, the player clicks on a
 * piece from the board ('src') and releases on the final destination ('dest')
 * to make a move. Legal move highlighting assists the user to make a correct
 * move. If the user enters an invalid move, an appropriate error message is
 * displayed at the status bar below the main board. At any given moment, a
 * player can decide to either continue, offer a draw, resign, undo the move, or
 * start a new game.
 *
 * Simply run <b>mingw32-make all_gui</b> on a Windows machine to make the
 * executable, then navigate to <i>chessCAMO/GUI/chessCAMO.exe</i> to play
 *
 * @note       Currently all standard chess rules are supported, except three
 *             move repetition & 50 move rule.
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include "chess.h"

using namespace sf;
using namespace std;

class ChessGUI
{
public:
    /**
     * @brief      Forms the pieces for the current board representation.
     *
     * @param      pieces     The pieces to draw on the board
     * @param[in]  pieceType  The piece type from which the drawn piece image is obtained
     * @param[in]  board      The current board representation
     */
    void formPieces(vector<Sprite> &pieces, const vector<Sprite> &pieceType, const vector<Piece*> &board)
    {
        for(const auto & elem : board)
        {
            int index = &elem - &board[0];
            if(elem->isRook())
                pieces[index] = elem->isPieceWhite() ? pieceType[1] : pieceType[7];
            else if(elem->isKnight())
                pieces[index] = elem->isPieceWhite() ? pieceType[2] : pieceType[8];
            else if(elem->isBishop())
                pieces[index] = elem->isPieceWhite() ? pieceType[3] : pieceType[9];
            else if(elem->isQueen())
                pieces[index] = elem->isPieceWhite() ? pieceType[4] : pieceType[10];
            else if(elem->isKing())
                pieces[index] = elem->isPieceWhite() ? pieceType[5] : pieceType[11];
            else if(elem->isPawn())
                pieces[index] = elem->isPieceWhite() ? pieceType[6] : pieceType[12];
            else
                pieces[index] = pieceType[0];

            int piece_width = pieces[index].getGlobalBounds().width, piece_height = pieces[index].getGlobalBounds().height;
            pieces[index].setPosition(piece_width/2 + piece_width * (index % 8), piece_height/2 + piece_height * (index / 8));
        }
    }

    /**
     * @brief      Forms the piece reservoir.
     *
     * @param      reservoir  The piece reservoir
     * @param[in]  pieceType  The piece type from which the drawn piece image is obtained
     */
    void formReservoir(vector<Sprite> &reservoir, const vector<Sprite> &pieceType)
    {
        reservoir[8] = pieceType[1];  // White Rook
        reservoir[6] = pieceType[2];  // White Knight
        reservoir[7] = pieceType[3];  // White Bishop
        reservoir[9] = pieceType[4];  // White Queen
        reservoir[5] = pieceType[6];  // White Pawn
        reservoir[3] = pieceType[7];  // Black Rook
        reservoir[1] = pieceType[8];  // Black Knight
        reservoir[2] = pieceType[9];  // Black Bishop
        reservoir[4] = pieceType[10]; // Black Queen
        reservoir[0] = pieceType[12]; // Black Pawn

        for(int i = 0; i < 10; i++)
        {
            FloatRect shape = reservoir[i].getGlobalBounds();
            reservoir[i].setPosition(541, shape.height/2 + (shape.height-12) * i);
            reservoir[i].setScale((shape.width-12)/shape.width, (shape.height-12)/shape.height);
        } 
    }

    /**
     * @brief      Draws text to the screen according to the provided parameters.
     *
     * @param      window       The window on which to draw
     * @param      text_object  The text object that is used
     * @param[in]  message      The message to write
     * @param[in]  size         The size of the characters
     * @param[in]  pos_x        The x position on the screen
     * @param[in]  pos_y        The y position on the screen
     * @param[in]  font         The font to use
     * @param[in]  color        The color that the text will be
     * @param[in]  style        The text's style
     */
    void drawText(RenderWindow &window, Text &text_object, string message, int size, unsigned int pos_x, unsigned int pos_y, Font font, Color color, unsigned int style)
    {
        text_object.setString(message);
        text_object.setCharacterSize(size);
        text_object.setFont(font);
        text_object.setFillColor(color);
        text_object.setStyle(style);
        text_object.setPosition(pos_x, pos_y);

        window.draw(text_object);
    }

    /**
     * @brief      Draws a rectangle in the interface window.
     *
     * @param      window       The window on which to draw
     * @param      rect_object  The rectangle object that is used
     * @param[in]  width       The width of the rectangle
     * @param[in]  height       The height of the rectangle
     * @param[in]  pos_x        The x position on the screen
     * @param[in]  pos_y        The y position on the screen
     * @param[in]  color        The color that the rectangle will be filled with
     * @param[in]  out_thick    The outline thickness (negative values go toward the center of rectangle)
     * @param[in]  out_color    The outline color
     */
    void drawRect(RenderWindow &window, RectangleShape &rect_object, float width, float height, unsigned int pos_x, unsigned int pos_y, Color color, int out_thick, Color out_color)
    {
        rect_object.setSize(Vector2f(width, height));
        rect_object.setPosition(pos_x, pos_y);
        rect_object.setFillColor(color);
        rect_object.setOutlineThickness(out_thick);
        rect_object.setOutlineColor(out_color);

        window.draw(rect_object);
    }

    /**
     * @brief      Gets the squares of legal moves.
     *
     * @param      legalMoves  The legal moves vector
     * @param[in]  src         The piece's source square
     * @param      chess       The chess object
     *
     * @pre        None
     * 
     * @post       A vector of legal moves squares by reference
     */
    void getLegalMoves(vector<int> &legalMoves, int src, Chess &chess)
    {
        vector<Piece*> board = chess.getBoard();

        for(unsigned int dest = 0; dest < board.size(); dest++)
            if(board[src]->isLegalMove(dest, chess))
                legalMoves.push_back(dest);
    }

    /**
     * @brief      Gets the squares of all pieces that match the turn type
     *
     * @param      sideSquares  The squares of the side whose turn it is
     *                          currently.
     * @param      chess        The chess object
     *
     * @pre        None
     *
     * @post       A vector of squares corresponding to pieces with same color
     *             as current turn by reference
     */
    void getSideToMoveSquares(vector<int> &sideSquares, Chess &chess)
    {
        for(const auto & elem : chess.getBoard())
            if(elem->getPieceColor() == chess.getTurn())
                sideSquares.push_back(elem->getPieceSquare());
    }

    /**
     * @brief      Resets the position to the corresponding position indicated
     *             by 'move_num'
     *
     * @param[in]  file_status     The file path for status information
     * @param[in]  file_promotion  The file path for promotion information
     * @param[in]  message         The message to use as a warning
     * @param[in]  move_num        The move number to revert the position to
     * @param      chess           The chess object
     * @param      sideSquares     The squares of pieces whose turn it will be (updated by reference)
     */
    void resetPosition(string file_status, string file_promotion, string message, int move_num, Chess &chess, vector<int> &sideSquares)
    {
        ofstream messageOut(file_status, ios::trunc);

        // fill up the first 23 lines with junk (status line reads 24th line)
        for(int i = 0; i < 23; i++)
            messageOut << "temp" << endl;

        // send the warning to the output stream, then close the stream
        messageOut << message << endl;
        messageOut.close();

        // set move counter accordingly
        chess.setNumMoves(move_num);

        // restore previous object
        chessCAMO::restoreObject(chess);

        // update side to move square highlighting
        sideSquares.clear();
        getSideToMoveSquares(sideSquares, chess);

        // default promotion is Queen (always, even if program ends after another piece is set)
        streamIO(file_promotion, "q", false, chess);
    }

    /**
     * @brief      Writes data to an output stream (file) and reads in data from
     *             an input stream (file) as need. Used for promotion and
     *             warning messages.
     *
     * @param[in]  file_path       The file path to read/write from
     * @param[in]  message         The message to write
     * @param[in]  draw_or_resign  Whether to apply the drawOrResign function
     * @param      chess           The chess object
     */
    void streamIO(string file_path, string message, bool draw_or_resign, Chess &chess)
    {
        streambuf *coutbuf;

        // open files
        ofstream messageOut(file_path, ios::trunc);
        ifstream messageIn(file_path);

        // redirect output stream to the output file
        coutbuf = cout.rdbuf();
        cout.rdbuf(messageOut.rdbuf());
    
        // send the message to the file (each character is written on new line)
        for(unsigned int i = 0; i < message.size(); i++)
            messageOut << message[i] << endl;

        // apply drawOrResign function if needed
        if(draw_or_resign)
            chessCAMO::drawOrResign(false, chess, messageIn);

        // redirect output stream back to cout
        cout.rdbuf(coutbuf);

        // close the opened files
        messageIn.close();
        messageOut.close();
    }
};

int main()
{
    // front end (GUI)
    ChessGUI chess_gui;

    // back end computation
    Chess chess;

    // Create 8x8 default board
    chess.boardInit();

    // Move & Side highlighting
    vector<int> legalMoves, sideSquares;

    // The window on which to draw the pieces
    RenderWindow window(VideoMode(600, 600), "chessCAMO", Style::Titlebar | Style::Close);

    // Pieces
    Texture blank, w1, w2, w3, w4, w5, w6, b1, b2, b3, b4, b5, b6;
    blank.loadFromFile("images/blank.png");
    w1.loadFromFile("images/wr.png");
    w2.loadFromFile("images/wn.png");
    w3.loadFromFile("images/wb.png");
    w4.loadFromFile("images/wq.png");
    w5.loadFromFile("images/wk.png");
    w6.loadFromFile("images/wp.png");
    b1.loadFromFile("images/br.png");
    b2.loadFromFile("images/bn.png");
    b3.loadFromFile("images/bb.png");
    b4.loadFromFile("images/bq.png");
    b5.loadFromFile("images/bk.png");
    b6.loadFromFile("images/bp.png");

    Sprite sBlank(blank), sWr(w1), sWn(w2), sWb(w3), sWq(w4), sWk(w5), sWp(w6), sBr(b1), sBn(b2), sBb(b3), sBq(b4), sBk(b5), sBp(b6);

    // updated continously throughout the program's life-cycle
    vector<Sprite> pieceType = {sBlank, sWr, sWn, sWb, sWq, sWk, sWp, sBr, sBn, sBb, sBq, sBk, sBp};
    vector<Sprite> pieces(64);
    vector<Sprite> reservoir(10);

    chess_gui.formPieces(pieces, pieceType, chess.getBoard());
    chess_gui.formReservoir(reservoir, pieceType);

    // text related information
    Text text, text_bottom, text_reservoir;
    Font font;

    // board sizes
    const int numRows = 10;
    const int numCols = 10;
    const int distance = 60; //distance between squares
    Vector2f size_rect(60.0, 60.0); // main squares
    RectangleShape rect(size_rect);

    // colors used for board square coloring
    Color color_dark(211, 211, 211, 255);
    Color color_yellow(255, 255, 153, 255);
    Color color_orange(255, 204, 153, 255);
    Color color_red(255, 204, 204, 255);

    // variables for non-event driven behaviour
    bool clicked = false, draw = false, resign = false, new_game = false;
    bool enable_side_highlighting = true, enable_move_highlighting = true;

    // files for promotion and status information updates
    string filename_promotion = "../GUI/object_states/promotion.txt";
    string filename_status = "../GUI/object_states/status.txt";

    // clear warning file
    chess_gui.streamIO(filename_status, " ", false, chess);

    // default promotion is Queen (always, even if program ends after another piece is set)
    chess_gui.streamIO(filename_promotion, "q", false, chess);

    // at game start highlight white pieces
    chess_gui.getSideToMoveSquares(sideSquares, chess); 

    while(window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;

        Event e;
        bool x_cursor = (30 < pos.x && pos.x < 510) || (540 < pos.x && pos.x < 600);
        bool y_cursor = 30 < pos.y && pos.y < 510;
        Cursor cursor;
        if( ( (x_cursor && y_cursor) || (540 < pos.x && pos.y < 0) ) && cursor.loadFromSystem(sf::Cursor::Hand) )
            window.setMouseCursor(cursor);
        else if(cursor.loadFromSystem(sf::Cursor::NotAllowed))
            window.setMouseCursor(cursor);

        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();

            if(e.type == Event::KeyPressed)
            {
                // reset the game
                if(e.key.code == Keyboard::S) 
                {
                    chess_gui.resetPosition(filename_status, filename_promotion, "New game started, good luck!", 0, chess, sideSquares);

                    // form the pieces for the new board representation
                    chess_gui.formPieces(pieces, pieceType, chess.getBoard());

                    new_game = true;
                    resign = false;
                }
                 
                // undo move
                else if(e.key.code == Keyboard::U && !resign) 
                {   
                    chess_gui.resetPosition(filename_status, filename_promotion, "Undo move applied", chess.getNumMoves()-1, chess, sideSquares);

                    // form the pieces for the new board representation
                    chess_gui.formPieces(pieces, pieceType, chess.getBoard());
                }
            }

            if(!chess.getCheckmate() && !chess.getStalemate())
            {
                if(e.type == Event::KeyPressed)
                {
                    // resign
                    if(e.key.code == Keyboard::Escape)
                    {
                        chess_gui.streamIO(filename_status, "r", true, chess);
                        resign = true;
                        new_game = false;
                    }

                    // draw
                    else if(e.key.code == Keyboard::D)
                    {
                        chess_gui.streamIO(filename_status, "dn", true, chess);
                        draw = true;
                        new_game = false;
                    }

                    // promotion handling
                    else if(e.key.code == Keyboard::R)
                        chess_gui.streamIO(filename_promotion, "r", false, chess);
                    else if(e.key.code == Keyboard::B)
                        chess_gui.streamIO(filename_promotion, "b", false, chess);
                    else if(e.key.code == Keyboard::N)
                        chess_gui.streamIO(filename_promotion, "n", false, chess);
                    else if(e.key.code == Keyboard::Q)
                        chess_gui.streamIO(filename_promotion, "q", false, chess);
                    
                    // move & side highlighting
                    else if(e.key.code == Keyboard::Num1)
                        enable_side_highlighting = !enable_side_highlighting; // toggle on/off
                    else if(e.key.code == Keyboard::Numpad1)
                        enable_move_highlighting = !enable_move_highlighting; // toggle on/off
                }

                if(e.type == Event::MouseButtonPressed)
                {
                    // allow click only on the board or reservoir pieces
                    bool x_cursor = (30 < pos.x && pos.x < 510) || (540 < pos.x && pos.x < 600);
                    bool y_cursor = 30 < pos.y && pos.y < 510;
                    if(e.mouseButton.button == Mouse::Left && x_cursor && y_cursor)
                    {
                        clicked = true;
                        draw = false;

                        for(iter = pieces.begin(); iter != pieces.end(); iter++)
                            if(iter->getGlobalBounds().contains(pos.x, pos.y))
                                src = iter - pieces.begin();

                        // reservoir piece to 'src' square converting
                        if( (540 <= pos.x && pos.x <= 600) && ( (30 <= pos.y && pos.y < 78) || (270 <= pos.y && pos.y < 318) ) ) { src = (int) 'p'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (78 <= pos.y && pos.y < 126) || (318 <= pos.y && pos.y < 366) ) ) { src = (int) 'n'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (126 <= pos.y && pos.y < 174) || (366 <= pos.y && pos.y < 414) ) ) { src = (int) 'o'; } // bishop
                        else if( (540 <= pos.x && pos.x <= 600) && ( (174 <= pos.y && pos.y < 222) || (414 <= pos.y && pos.y < 462) ) ) { src = (int) 'r'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (222 <= pos.y && pos.y < 270) || (462 <= pos.y && pos.y <= 510) ) ) { src = (int) 'q'; }

                        // if clicked on main board piece, get its legal moves and remove side to move higlighting
                        if(0 <= src && src <= 63)
                        {
                            chess_gui.getLegalMoves(legalMoves, src, chess); // updates legalMoves by reference
                            sideSquares.clear();
                        }
                    }
                }

                if(e.type == Event::MouseButtonReleased)
                {
                    if(e.mouseButton.button == Mouse::Left)
                    {
                        clicked = false;
                        dest = int((pos.x / size_rect.x) - 0.5) + 8 * int((pos.y / size_rect.y) - 0.5);

                        // make the move and output messages to the correct output stream. 
                        // input stream is needed in case of promotion
                        ofstream messageOut(filename_status, ios::trunc);
                        streambuf *coutbuf = cout.rdbuf();
                        cout.rdbuf(messageOut.rdbuf());

                        ifstream in(filename_promotion);
                        chess.makeMove(src, dest, in);
                        in.close();

                        cout.rdbuf(coutbuf);
                        messageOut.close();

                        // reform the pieces on the board based on the new move made
                        chess_gui.formPieces(pieces, pieceType, chess.getBoard());

                        // clear all legal moves for next cycle and change the side to move highighting
                        if(0 <= src && src <= 63)
                        {
                            legalMoves.clear();
                            chess_gui.getSideToMoveSquares(sideSquares, chess); // updates sideSquares by reference
                        }
                    }
                }

                // piece drag & drop functionality (only applied to main board pieces - not reservoir pieces)
                if(clicked)
                {
                    if(0 <= src && src <= 63)
                        pieces[src].setPosition(pos.x - size_rect.x / 2, pos.y - size_rect.y / 2);
                }
            }  
        }

        window.clear();

        // -------- User Interface Drawing -------- //
        if(font.loadFromFile("font/arial.ttf"))
        {
            // -------- Status Panel -------- //
            chess_gui.drawText(window, text_bottom, "Status:", 20, 30, 560, font, Color::Green, Text::Regular);

            string first_message = chess.getTurn() == WHITE ? "White's move" : "Black's move", second_message;
            chess_gui.drawText(window, text_bottom, first_message, 20, 120, 560, font, Color::Cyan, Text::Regular);

            // warning and status update messages are always on line 24 (or line 26 for resign / line 50 for draw)
            ifstream messageIn(filename_status);
            int line_num;
            if(draw && !new_game) { line_num = 50; }
            else if(resign && !new_game) { line_num = 26; }
            else { line_num = 24; }

            for(int i = 0; i < line_num; i++)
               getline(messageIn, second_message, '\n');
            messageIn.close();

            // only print warning if it is one of the following
            string possible_warnings[] = {"White won by Checkmate!", "Black won by Checkmate!",
                                          "White has no moves -> Game is Drawn!", "Black has no moves -> Game is Drawn!", 
                                          "Game drawn by agreement", "Draw rejected. Game continues...",
                                          "Check!", "Double Check!", "You must move your king!", 
                                          "Invalid move! Try again...", "Undo move applied", "New game started, good luck!",
                                          "White resigned => Black wins", "Black resigned => White wins"};
            for(auto warning : possible_warnings)
                if(warning == second_message)
                    chess_gui.drawText(window, text_bottom, second_message, 20, 260, 560, font, Color::Yellow, Text::Regular);       

            // -------------- main grid -------------- //
            for(int i = 0; i < numRows; i++)
            {
                for(int j = 0; j < numCols; j++)
                {
                    // top and bottom rows
                    if((i == 0 || i == 9) && (1 <= j && j <= 8))
                    {
                        if (i == 0)
                            chess_gui.drawRect(window, rect, size_rect.x, size_rect.y/2, j * distance - size_rect.x/2, i * distance, (i + j) % 2 != 0 ? color_yellow : color_orange, 0, Color::Transparent);
                        else
                            chess_gui.drawRect(window, rect, size_rect.x, size_rect.y/2, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 == 0 ? color_yellow : color_orange, 0, Color::Transparent);
                    }

                    // left and right columns
                    else if((j == 0 || j == 9) && (1 <= i && i <= 8))
                    {
                        if (j == 0)
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y, j * distance, i * distance - size_rect.y/2, (i + j) % 2 != 0 ? color_yellow : color_orange, 0, Color::Transparent);
                        else
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 == 0 ? color_yellow : color_orange, 0, Color::Transparent);
                    }

                    // corners
                    else if((i == 0 && j == 0) || (i == 9 && j == 0) || (i == 0 && j == 9) || (i == 9 && j == 9))
                    {
                        if(i == 0 && j == 0)
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance, i * distance, color_red, 0, Color::Transparent);
                        else if(i == 0 && j == 9)
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance - size_rect.x/2, i * distance, color_red, 0, Color::Transparent);
                        else if(i == 9 && j == 0)
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance, i * distance - size_rect.y/2, color_red, 0, Color::Transparent);
                        else if(i == 9 && j == 9)
                            chess_gui.drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance - size_rect.x/2, i * distance - size_rect.y/2, color_red, 0, Color::Transparent);
                    }

                    // middle squares
                    else
                        chess_gui.drawRect(window, rect, size_rect.x, size_rect.y, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 != 0 ? color_dark : Color::White, 0, Color::Transparent);

                    // legal move highlighting
                    if(!legalMoves.empty() && enable_move_highlighting)
                    {
                        auto found = std::find(legalMoves.begin(), legalMoves.end(), (i-1) * 8 + (j-1));
                        if(found != legalMoves.end() && chess.getBoard()[src]->getPieceColor() == chess.getTurn() && i != 0 && j != 0 && i != 9 && j != 9)
                            chess_gui.drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), -2, Color::Cyan);
                        else { chess_gui.drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), 0, Color::Transparent); }
                    }

                    if(!sideSquares.empty() && enable_side_highlighting)
                    {
                        auto found = std::find(sideSquares.begin(), sideSquares.end(), (i-1) * 8 + (j-1));
                        if(found != sideSquares.end() && i != 0 && j != 0 && i != 9 && j != 9)
                            chess_gui.drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), -1, Color(178, 255, 102));
                        else { chess_gui.drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), 0, Color::Transparent); }
                    }
                }
            }

            // main grid text (files and ranks)
            string piece_file[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
            string piece_rank[8] = {"8", "7", "6", "5", "4", "3", "2", "1"};

            // top and bottom rows
            for(int j = 1; j < numCols-1; j++)
            {
                int x = (j * distance) - 8, y1 = 5, y2 = size_rect.y * (numRows - 1) - 25;     
                chess_gui.drawText(window, text, piece_file[j - 1], 20, x, y1, font, Color::Black, Text::Bold);
                chess_gui.drawText(window, text, piece_file[j - 1], 20, x, y2, font, Color::Black, Text::Bold);
            }

            // left and right columns
            for(int i = 1; i < numRows-1; i++)
            {     
                int y = (i * distance) - 10, x1 = 10, x2 = size_rect.x * (numCols - 1) - 20;
                chess_gui.drawText(window, text, piece_rank[i - 1], 20, x1, y, font, Color::Black, Text::Bold);
                chess_gui.drawText(window, text, piece_rank[i - 1], 20, x2, y, font, Color::Black, Text::Bold);
            }

            // reservoir rects
            for(int k = 0; k <= 11; k++)
            {
                if(k == 0 || k == 11)
                    chess_gui.drawRect(window, rect, size_rect.x, size_rect.y/2, 540, k == 0 ? 0 : 510, Color(204, 255, 204), 0, Color::Transparent);
                else 
                    chess_gui.drawRect(window, rect, size_rect.x, size_rect.y-12, 540, size_rect.x/2 + (k-1) * (distance-12), k > 5 ? Color(255, 255, 255) : Color(224, 224, 224), -1, Color::Black);
            }

            // reservoir text
            chess_gui.drawText(window, text_reservoir, "RSVR", 20, 541, 5, font, Color::Black, Text::Bold);

            vector<pair<int, char>> current_resevoir = chess.getReservoir();
            vector<int> num_left(10);
            for(unsigned int i = 0; i < num_left.size(); i++)
                num_left[i] = current_resevoir[i].first;

            for(unsigned int i = 0; i < num_left.size(); i++)
                chess_gui.drawText(window, text_reservoir, "x" + to_string(num_left[i]), 12, 585, 45 + i*48, font, i > 4 ? Color(0, 102, 0) : Color(204, 0, 0), Text::Regular);

            chess_gui.drawText(window, text_reservoir, "RSVR", 20, 541, 515, font, Color::Black, Text::Bold);
        }
        else { cout << "failed to load font file" << endl; }

        // Display the pieces on the window of the GUI pieces
        for(const auto &piece : pieces)
            window.draw(piece);

        // reservoir
        for(const auto &piece : reservoir)
            window.draw(piece);

        window.display();
    }

    cout.clear();

    return 0;
}