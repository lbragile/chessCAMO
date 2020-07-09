#include <SFML/Graphics.hpp>
#include <vector>
#include "chess.h"

using namespace sf;
using namespace std;

void drawPieces(vector<Sprite> & pieces, const vector<Sprite> & pieceType, const vector<Piece*> & board)
{
    FloatRect shape;
    int i = 0;
    for(const auto & elem : board)
    {
        if(elem->isRook())
            pieces[i] = elem->isPieceWhite() ? pieceType[1] : pieceType[7];
        else if(elem->isKnight())
            pieces[i] = elem->isPieceWhite() ? pieceType[2] : pieceType[8];
        else if(elem->isBishop())
            pieces[i] = elem->isPieceWhite() ? pieceType[3] : pieceType[9];
        else if(elem->isQueen())
            pieces[i] = elem->isPieceWhite() ? pieceType[4] : pieceType[10];
        else if(elem->isKing())
            pieces[i] = elem->isPieceWhite() ? pieceType[5] : pieceType[11];
        else if(elem->isPawn())
            pieces[i] = elem->isPieceWhite() ? pieceType[6] : pieceType[12];
        else
            pieces[i] = pieceType[0];

        shape = pieces[i].getGlobalBounds();
        pieces[i].setPosition(shape.width/2 + shape.width * (i % 8), shape.height/2 + shape.height * (i / 8));
        i++;
    }
}

void drawReservoir(vector<Sprite> & reservoir, const vector<Sprite> & pieceType)
{
    FloatRect shape;

    reservoir[8] = pieceType[1];  // Wr
    reservoir[6] = pieceType[2];  // Wn
    reservoir[7] = pieceType[3];  // Wb
    reservoir[9] = pieceType[4];  // Wq
    reservoir[5] = pieceType[6];  // Wp
    reservoir[3] = pieceType[7];  // Br
    reservoir[1] = pieceType[8];  // Bn
    reservoir[2] = pieceType[9];  // Bb
    reservoir[4] = pieceType[10]; // Bq
    reservoir[0] = pieceType[12]; // Bp

    for(int i = 0; i < 10; i++)
    {
        shape = reservoir[i].getGlobalBounds();
        reservoir[i].setPosition(541, shape.height/2 + (shape.height-12) * i);
        reservoir[i].setScale((shape.width-12)/shape.width, (shape.height-12)/shape.height);
    } 
}


void getLegalMoves(vector<int> & legalMoves, int src, Chess &chess)
{
    vector<Piece*> board = chess.getBoard();

    for(unsigned int dest = 0; dest < board.size(); dest++)
        if(board[src]->isLegalMove(dest, chess))
            legalMoves.push_back(dest);
}

void getSideToMoveSquares(vector<int> & sideSquares, Chess &chess)
{
    for(const auto & elem : chess.getBoard())
        if(elem->getPieceColor() == chess.getTurn())
            sideSquares.push_back(elem->getPieceSquare());
}

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

void drawRect(RenderWindow &window, RectangleShape &rect_object, float size_x, float size_y, unsigned int pos_x, unsigned int pos_y, Color color, int out_thick, Color out_color)
{
    rect_object.setSize(Vector2f(size_x, size_y));
    rect_object.setPosition(pos_x, pos_y);
    rect_object.setFillColor(color);
    rect_object.setOutlineThickness(out_thick);
    rect_object.setOutlineColor(out_color);
    window.draw(rect_object);
}

int main()
{
    Chess chess;

    // Create 8x8 default board
    chess.boardInit();

    vector<int> legalMoves, sideSquares;

    RenderWindow window(VideoMode(600, 600), "chessCAMO", Style::Titlebar | Style::Close);

    // pieces
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

    Sprite sBlank(blank), sWr(w1), sWn(w2), sWb(w3), sWq(w4), sWk(w5), sWp(w6),
        sBr(b1), sBn(b2), sBb(b3), sBq(b4), sBk(b5), sBp(b6);

    vector<Sprite> pieceType = {sBlank, sWr, sWn, sWb, sWq, sWk, sWp, sBr, sBn, sBb, sBq, sBk, sBp};
    vector<Sprite> pieces(64);
    vector<Sprite> reservoir(10);

    drawReservoir(reservoir, pieceType);
    drawPieces(pieces, pieceType, chess.getBoard());

    Vector2f size_rect(60.0, 60.0); // main squares
    Vector2f lr_rect(size_rect.x / 2, size_rect.y); // left and right columns
    Vector2f tb_rect(size_rect.x, size_rect.y / 2); // top and bottom rows

    RectangleShape rect(size_rect);

    Text text, text_side, text_bottom, text_reservoir;
    Font font;
    const int numRows = 10;
    const int numCols = 10;
    const int distance = 60; //distance between squares
    Color color_dark(211, 211, 211, 255);
    Color color_yellow(255, 255, 153, 255);
    Color color_orange(255, 204, 153, 255);
    Color color_red(255, 204, 204, 255);
    bool clicked = false, draw = false, resign = false;
    bool enable_side_highlighting = true, enable_move_highlighting = true;

    string filename = "../GUI/object_states/promotion.txt";
    string filename_status = "../GUI/object_states/status.txt";

    // default promotion is Queen (always, even if program ends after another
    // piece is set)
    ofstream outFile(filename, ios::trunc);
    outFile << 'q' << endl;
    outFile.close();

    // clear warning file
    ofstream messageOut(filename_status, ios::trunc);
    messageOut << "";
    messageOut.close();

    getSideToMoveSquares(sideSquares, chess); // at game start highlight white pieces

    while(window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;

        Event e;

        Cursor cursor;
        if( ( (( ((30 < pos.x && pos.x < 510) || (540 < pos.x && pos.x < 600)) && ( (30 < pos.y && pos.y < 510) )) || (540 < pos.x && pos.y < 0) ) && cursor.loadFromSystem(sf::Cursor::Hand) ))
            window.setMouseCursor(cursor);
        else if(cursor.loadFromSystem(sf::Cursor::NotAllowed))
            window.setMouseCursor(cursor);

        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();

            // // reset the game
            // if(e.type == Event::KeyPressed && e.key.code == Keyboard::S)
            // {
            //     ofstream messageOut(filename_status, ios::trunc);

            //     // fill up the first 20 lines with junk (status line reads 21st line)
            //     for(int i = 0; i < 20; i++)
            //     {
            //         messageOut << "temp" << endl;
            //     }
            //     messageOut << "New Game Started, Good Luck!" << endl;
                
            //     messageOut.close();

            //     // restore and print the board
            //     chessCAMO::restoreObject(0, chess);
            //     drawPieces(pieces, pieceType, chess.getBoard());
            // }

            if(!chess.getCheckmate() && !chess.getStalemate())
            {
                if(e.type == Event::KeyPressed)
                {
                    if(e.key.code == Keyboard::U) 
                    {   
                        ofstream messageOut(filename_status, ios::trunc);
                        // fill up the first 20 lines with junk (status line reads 21st line)
                        for(int i = 0; i < 23; i++)
                            messageOut << "temp" << endl;
                        messageOut << "Undo move applied" << endl;
                        messageOut.close();

                        // decrement move counter by 1 since an illegal move was made 
                        chess.setNumMoves(chess.getNumMoves()-1);

                        // restore previous object
                        chessCAMO::restoreObject(chess);

                        drawPieces(pieces, pieceType, chess.getBoard());
                        sideSquares.clear();
                        getSideToMoveSquares(sideSquares, chess);
                    }

                    else if(e.key.code == Keyboard::Escape)
                    {
                        ofstream messageOut(filename_status, ios::trunc);
                        ifstream messageIn(filename_status);

                        streambuf *coutbuf = cout.rdbuf();
                        cout.rdbuf(messageOut.rdbuf());

                        messageOut << 'r' << endl;
                        chessCAMO::drawOrResign(false, chess, messageIn);

                        cout.rdbuf(coutbuf);
                        messageIn.close();
                        messageOut.close();

                        resign = true;
                    }

                    else if(e.key.code == Keyboard::D)
                    {
                        ofstream messageOut(filename_status, ios::trunc);
                        ifstream messageIn(filename_status);

                        streambuf *coutbuf = cout.rdbuf();
                        cout.rdbuf(messageOut.rdbuf());

                        messageOut << 'd' << endl;
                        messageOut << 'n' << endl;
                        chessCAMO::drawOrResign(false, chess, messageIn);

                        cout.rdbuf(coutbuf);
                        messageIn.close();
                        messageOut.close();

                        draw = true;
                    }

                    else if(e.key.code == Keyboard::R)
                    {
                        ofstream outFile(filename, ios::trunc);
                        outFile << 'r' << endl;
                        outFile.close();
                    }
                    else if(e.key.code == Keyboard::B)
                    {
                        ofstream outFile(filename, ios::trunc);
                        outFile << 'b' << endl;
                        outFile.close();
                    }
                    else if(e.key.code == Keyboard::N)
                    {
                        ofstream outFile(filename, ios::trunc);
                        outFile << 'n' << endl;
                        outFile.close();
                    }
                    else if(e.key.code == Keyboard::Q)
                    {
                        ofstream outFile(filename, ios::trunc);
                        outFile << 'q' << endl;
                        outFile.close();
                    }

                    else if(e.key.code == Keyboard::Num1)
                        enable_side_highlighting = !enable_side_highlighting;

                    else if(e.key.code == Keyboard::Numpad1)
                        enable_move_highlighting = !enable_move_highlighting;
                }

                if(e.type == Event::MouseButtonPressed)
                {
                    if(e.mouseButton.button == Mouse::Left && ((30 < pos.x && pos.x < 510) || (540 < pos.x && pos.x < 600)) && 30 < pos.y && pos.y < 510)
                    {
                        clicked = true;
                        draw = false;
                        for(iter = pieces.begin(); iter != pieces.end(); iter++)
                            if(iter->getGlobalBounds().contains(pos.x, pos.y))
                                src = iter - pieces.begin();

                        // reservoir
                        if( (540 <= pos.x && pos.x <= 600) && ( (30 <= pos.y && pos.y < 78) || (270 <= pos.y && pos.y < 318) ) ) { src = (int) 'p'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (78 <= pos.y && pos.y < 126) || (318 <= pos.y && pos.y < 366) ) ) { src = (int) 'n'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (126 <= pos.y && pos.y < 174) || (366 <= pos.y && pos.y < 414) ) ) { src = (int) 'o'; } // bishop
                        else if( (540 <= pos.x && pos.x <= 600) && ( (174 <= pos.y && pos.y < 222) || (414 <= pos.y && pos.y < 462) ) ) { src = (int) 'r'; }
                        else if( (540 <= pos.x && pos.x <= 600) && ( (222 <= pos.y && pos.y < 270) || (462 <= pos.y && pos.y <= 510) ) ) { src = (int) 'q'; }

                        if(0 <= src && src <= 63)
                        {
                            getLegalMoves(legalMoves, src, chess); // updates legalMoves by reference
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

                        ofstream messageOut(filename_status, ios::trunc);
                        streambuf *coutbuf = cout.rdbuf();
                        cout.rdbuf(messageOut.rdbuf());

                        ifstream in(filename);
                        chess.makeMove(src, dest, in);
                        in.close();

                        cout.rdbuf(coutbuf);
                        messageOut.close();

                        drawPieces(pieces, pieceType, chess.getBoard());

                        // clear all legal moves for next cycle
                        if(0 <= src && src <= 63)
                        {
                            legalMoves.clear();
                            getSideToMoveSquares(sideSquares, chess); // updates sideSquares by reference
                        }
                    }
                }

                if(clicked)
                {
                    if(0 <= src && src <= 63)
                        pieces[src].setPosition(pos.x - size_rect.x / 2, pos.y - size_rect.y / 2);
                }
            }  
        }

        window.clear();

        // board
        if(font.loadFromFile("font/arial.ttf"))
        {
            // -------- side & bottom panels -------- //
            // text
            drawText(window, text_bottom, "Status:", 20, 30, 560, font, Color::Green, Text::Regular);

            string first_message = chess.getTurn() == WHITE ? "White's move" : "Black's move", second_message;
            drawText(window, text_bottom, first_message, 20, 120, 560, font, Color::Cyan, Text::Regular);

            ifstream messageIn(filename_status);
            // warning and status update messages are always on line 24 (or line
            // 26 for resign / line 50 for draw)
            int line_num;
            if(draw) { line_num = 50; }
            else if(resign) { line_num = 26; }
            else { line_num = 24; }

            for(int i = 0; i < line_num; i++)
               getline(messageIn, second_message, '\n');
            messageIn.close();

            // only print warning if it is one of the following
            string possible_warnings[] = {"White won by Checkmate!", "Black won by Checkmate!",
                                          "White has no moves -> Game is Drawn!", "Black has no moves -> Game is Drawn!", 
                                          "Game drawn by agreement", "Draw rejected. Game continues...",
                                          "Check!", "Double Check!", "You must move your king!", 
                                          "Invalid move! Try again...", "Undo move applied",
                                          "White resigned => Black wins", "Black resigned => White wins"};
            for(auto warning : possible_warnings)
                if(warning == second_message)
                    drawText(window, text_bottom, second_message, 20, 260, 560, font, Color::Yellow, Text::Regular);       

            // -------------- main grid -------------- //
            for(int i = 0; i < numRows; i++)
            {
                for(int j = 0; j < numCols; j++)
                {
                    // top and bottom rows
                    if((i == 0 || i == 9) && (1 <= j && j <= 8))
                    {
                        if (i == 0)
                            drawRect(window, rect, size_rect.x, size_rect.y/2, j * distance - size_rect.x/2, i * distance, (i + j) % 2 != 0 ? color_yellow : color_orange, 0, Color::Transparent);
                        else
                            drawRect(window, rect, size_rect.x, size_rect.y/2, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 == 0 ? color_yellow : color_orange, 0, Color::Transparent);
                    }

                    // left and right columns
                    else if((j == 0 || j == 9) && (1 <= i && i <= 8))
                    {
                        if (j == 0)
                            drawRect(window, rect, size_rect.x/2, size_rect.y, j * distance, i * distance - size_rect.y/2, (i + j) % 2 != 0 ? color_yellow : color_orange, 0, Color::Transparent);
                        else
                            drawRect(window, rect, size_rect.x/2, size_rect.y, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 == 0 ? color_yellow : color_orange, 0, Color::Transparent);
                    }

                    // corners
                    else if((i == 0 && j == 0) || (i == 9 && j == 0) || (i == 0 && j == 9) || (i == 9 && j == 9))
                    {
                        if(i == 0 && j == 0)
                            drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance, i * distance, color_red, 0, Color::Transparent);
                        else if(i == 0 && j == 9)
                            drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance - size_rect.x/2, i * distance, color_red, 0, Color::Transparent);
                        else if(i == 9 && j == 0)
                            drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance, i * distance - size_rect.y/2, color_red, 0, Color::Transparent);
                        else if(i == 9 && j == 9)
                            drawRect(window, rect, size_rect.x/2, size_rect.y/2, j * distance - size_rect.x/2, i * distance - size_rect.y/2, color_red, 0, Color::Transparent);
                    }

                    // middle squares
                    else
                        drawRect(window, rect, size_rect.x, size_rect.y, j * distance - size_rect.x/2, i * distance - size_rect.y/2, (i + j) % 2 != 0 ? color_dark : Color::White, 0, Color::Transparent);

                    // legal move highlighting
                    if(!legalMoves.empty() && enable_move_highlighting)
                    {
                        auto found = std::find(legalMoves.begin(), legalMoves.end(), (i-1) * 8 + (j-1));
                        if(found != legalMoves.end() && chess.getBoard()[src]->getPieceColor() == chess.getTurn() && i != 0 && j != 0 && i != 9 && j != 9)
                            drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), -2, Color::Cyan);
                        else { drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), 0, Color::Transparent); }
                    }

                    if(!sideSquares.empty() && enable_side_highlighting)
                    {
                        auto found = std::find(sideSquares.begin(), sideSquares.end(), (i-1) * 8 + (j-1));
                        if(found != sideSquares.end() && i != 0 && j != 0 && i != 9 && j != 9)
                            drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), -1, Color(178, 255, 102));
                        else { drawRect(window, rect, rect.getSize().x, rect.getSize().y, rect.getPosition().x, rect.getPosition().y, rect.getFillColor(), 0, Color::Transparent); }
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
                drawText(window, text, piece_file[j - 1], 20, x, y1, font, Color::Black, Text::Bold);
                drawText(window, text, piece_file[j - 1], 20, x, y2, font, Color::Black, Text::Bold);
            }

            // left and right columns
            for(int i = 1; i < numRows-1; i++)
            {     
                int y = (i * distance) - 10, x1 = 10, x2 = size_rect.x * (numCols - 1) - 20;
                drawText(window, text, piece_rank[i - 1], 20, x1, y, font, Color::Black, Text::Bold);
                drawText(window, text, piece_rank[i - 1], 20, x2, y, font, Color::Black, Text::Bold);
            }

            // reservoir rects
            for(int k = 0; k <= 11; k++)
            {
                if(k == 0 || k == 11)
                    drawRect(window, rect, size_rect.x, size_rect.y/2, 540, k == 0 ? 0 : 510, Color(204, 255, 204), 0, Color::Transparent);
                else 
                    drawRect(window, rect, size_rect.x, size_rect.y-12, 540, size_rect.x/2 + (k-1) * (distance-12), k > 5 ? Color(255, 255, 255) : Color(224, 224, 224), -1, Color::Black);
            }

            // reservoir text
            drawText(window, text_reservoir, "RSVR", 20, 541, 5, font, Color::Black, Text::Bold);

            vector<pair<int, char>> current_resevoir = chess.getReservoir();
            vector<int> num_left(10);
            for(unsigned int i = 0; i < num_left.size(); i++)
                num_left[i] = current_resevoir[i].first;

            for(unsigned int i = 0; i < num_left.size(); i++)
                drawText(window, text_reservoir, "x" + to_string(num_left[i]), 12, 585, 45 + i*48, font, i > 4 ? Color(0, 102, 0) : Color(204, 0, 0), Text::Regular);

            drawText(window, text_reservoir, "RSVR", 20, 541, 515, font, Color::Black, Text::Bold);
        }
        else { cout << "failed to load font file" << endl; }

        // pieces
        for (const auto & piece : pieces)
            window.draw(piece);

        // reservoir
        for (const auto & piece : reservoir)
            window.draw(piece);

        window.display();
    }

    cout.clear();

    return 0;
}