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
        reservoir[i].setPosition(545, shape.height/2 + (shape.height-12) * i);
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

int main()
{
    Chess chess;

    // Create 8x8 default board
    chess.boardInit();

    vector<int> legalMoves;

    RenderWindow window(VideoMode(840, 600), "chessCAMO", Style::Titlebar | Style::Close);

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
    bool clicked = false, draw = false, resign = false; // moved = false;
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

    while(window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;

        Event e;

        Cursor cursor;
        if( ( (( (30 < pos.x && pos.x < 510)) && ( (30 < pos.y && pos.y < 510) )) || (540 < pos.x && pos.y < 0) ) && cursor.loadFromSystem(sf::Cursor::Hand) )
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
                        for(int i = 0; i < 20; i++)
                            messageOut << "temp" << endl;
                        messageOut << "Undo move applied" << endl;
                        messageOut.close();

                        // decrement move counter by 1 since an illegal move was made 
                        chess.setNumMoves(chess.getNumMoves()-1);

                        // restore previous object
                        chessCAMO::restoreObject(chess.getNumMoves(), chess);

                        drawPieces(pieces, pieceType, chess.getBoard());
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
                }

                if(e.type == Event::MouseButtonPressed)
                {
                    if(e.mouseButton.button == Mouse::Left && 30 < pos.x && pos.x < 510 && 30 < pos.y && pos.y < 510)
                    {
                        clicked = true;
                        draw = false;
                        for(iter = pieces.begin(); iter != pieces.end(); iter++)
                            if(iter->getGlobalBounds().contains(pos.x, pos.y))
                                src = iter - pieces.begin();

                        getLegalMoves(legalMoves, src, chess); // updates legalMoves by reference
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
                        while(!legalMoves.empty())
                            legalMoves.pop_back();
                    }
                }

                if(clicked)
                    pieces[src].setPosition(pos.x - size_rect.x / 2, pos.y - size_rect.y / 2);
            }  
        }

        window.clear();

        // board
        if(font.loadFromFile("font/arial.ttf"))
        {
            // -------- side & bottom panels -------- //
            // text
            text_bottom.setCharacterSize(20);
            text_bottom.setFont(font);

            text_bottom.setFillColor(Color::Green);
            text_bottom.setPosition(30, 560);
            text_bottom.setString("Status:  ");
            text_bottom.setStyle(Text::Regular);
            window.draw(text_bottom);

            string first_message = chess.getTurn() == WHITE ? "White's move" : "Black's move", second_message;
            
            text_bottom.setFillColor(Color::Cyan);
            text_bottom.setPosition(120, 560);
            text_bottom.setString(first_message);
            text_bottom.setStyle(Text::Regular);
            window.draw(text_bottom);               

            ifstream messageIn(filename_status);
            // warning and status update messages are always on line 21 (or line
            // 23 for resign / line 44 for draw)
            int line_num;
            if(draw)
                line_num = 44;
            else if(resign)
                line_num = 23;
            else
                line_num = 21;

            for(int i = 0; i < line_num; i++)
               getline(messageIn, second_message, '\n');
            messageIn.close();

            // only print warning if it is one of the following
            string possible_warnings[] = {"White won by Checkmate!", "Black won by Checkmate!",
                                          "White has no moves -> Game is Drawn!", "Black has no moves -> Game is Drawn!", 
                                          "Game drawn by agreement", "Draw rejected. Game continues...",
                                          "You are in check! Try again...", "You are in double check! Try again...",
                                          "Check!", "Double Check!", "You must move your king!", 
                                          "Invalid move! Try again...", "Undo move applied",
                                          "White resigned => Black wins", "Black resigned => White wins"};
            for(auto warning : possible_warnings)
            {
                if(warning == second_message)
                {
                    text_bottom.setFillColor(Color::Yellow);
                    text_bottom.setPosition(260, 560);
                    text_bottom.setString(second_message);
                    text_bottom.setStyle(Text::Regular);
                    window.draw(text_bottom);
                }
            }

            text_side.setCharacterSize(20);
            text_side.setFont(font);
            text_side.setFillColor(Color::White);

            text_side.setPosition(615, 50);
            text_side.setString("To promote, first type \none of the following \ncharacters, then make \nthe move:\n\n"
                                "1. Q (or q)\n\n"
                                "2. R (or r)\n\n"
                                "3. N (or n)\n\n"
                                "4. B (or b)\n\n");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(715, 165);
            text_side.setString("=> Queen\n\n"
                                "=> Rook\n\n"
                                "=> Knight\n\n"
                                "=> Bishop");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(615, 400);
            text_side.setString("Resign:\n\n"
                                "Offer draw:\n\n"
                                "Undo Move:\n\n");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(740, 400);
            text_side.setString("Escape\n\n"
                                "D (or d)\n\n"
                                "U (or u)\n\n");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);            

            // -------------- main grid -------------- //
            // center the text inside it's bounding box
            FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width/2.0, textRect.top + textRect.height/2.0);

            text.setCharacterSize(20);
            text.setFont(font);
            text.setFillColor(Color::Black);

            string piece_file[8] = {"A", "B", "C", "D", "E", "F", "G", "H"};
            string piece_rank[8] = {"8", "7", "6", "5", "4", "3", "2", "1"};

            for(int i = 0; i < numRows; i++)
            {
                for(int j = 0; j < numCols; j++)
                {
                    // top and bottom rows
                    if((i == 0 || i == 9) && (1 <= j && j <= 8))
                    {
                        int x = j * distance, y1 = size_rect.y/4, y2 = size_rect.y * (numRows - 1.25);
                        i == 0 ? text.setPosition(x, y1) : text.setPosition(x, y2);
                        text.setString(piece_file[j - 1]);
                        
                        rect.setSize(Vector2f(size_rect.x, size_rect.y/2));
                        if (i == 0)
                        {
                            text.setStyle(Text::Bold);
                            rect.setFillColor((i + j) % 2 != 0 ? color_yellow : color_orange);
                            rect.setPosition(j * distance - size_rect.x/2, i * distance);
                        }
                        else
                        {
                            text.setStyle(Text::Bold);
                            rect.setFillColor((i + j) % 2 == 0 ? color_yellow : color_orange);
                            rect.setPosition(j * distance - size_rect.x/2, i * distance - size_rect.y/2);
                        }
                    }

                    // left and right columns
                    else if((j == 0 || j == 9) && (1 <= i && i <= 8))
                    {
                        int y = i * distance, x1 = size_rect.x/4, x2 = size_rect.x * (numCols - 1.25);
                        j == 0 ? text.setPosition(x1, y) : text.setPosition(x2, y);
                        text.setString(piece_rank[i - 1]);

                        rect.setSize(Vector2f(size_rect.x/2, size_rect.y));
                        if (j == 0)
                        {
                            text.setStyle(Text::Regular);
                            rect.setFillColor((i + j) % 2 != 0 ? color_yellow : color_orange);
                            rect.setPosition(j * distance, i * distance - size_rect.y/2);
                        }
                        else
                        {
                            text.setStyle(Text::Bold);
                            rect.setFillColor((i + j) % 2 == 0 ? color_yellow : color_orange);
                            rect.setPosition(j * distance - size_rect.x/2, i * distance - size_rect.y/2);
                        }
                    }

                    // corners
                    else if((i == 0 && j == 0) || (i == 9 && j == 0) || (i == 0 && j == 9) || (i == 9 && j == 9))
                    {
                        if(i == 0 && j == 0)
                            rect.setPosition(j * distance, i * distance);
                        else if(i == 0 && j == 9)
                            rect.setPosition(j * distance - size_rect.x/2, i * distance);
                        else if(i == 9 && j == 0)
                            rect.setPosition(j * distance, i * distance - size_rect.y/2);
                        else if(i == 9 && j == 9)
                            rect.setPosition(j * distance - size_rect.x/2, i * distance - size_rect.y/2);

                        rect.setSize(Vector2f(size_rect.x/2, size_rect.y/2));
                        rect.setFillColor(color_red);
                    }

                    // middle squares
                    else
                    {
                        rect.setSize(Vector2f(size_rect.x, size_rect.y));
                        rect.setFillColor((i + j) % 2 != 0 ? color_dark : Color::White);
                        rect.setPosition(j * distance - size_rect.x/2, i * distance - size_rect.y/2);
                    }

                    // legal move highlighting
                    auto found = std::find(legalMoves.begin(), legalMoves.end(), (i-1) * 8 + (j-1));
                    if(found != legalMoves.end() && chess.getBoard()[src]->getPieceColor() == chess.getTurn() && 
                       i != 0 && j != 0 && i != 9 && j != 9)
                    {
                        rect.setOutlineThickness(-2); // -1 means towards the center 1 pixel
                        rect.setOutlineColor(Color::Cyan);
                        window.draw(rect);
                    }

                    else
                    {
                        rect.setOutlineThickness(0);
                        window.draw(rect);
                    }

                    window.draw(text);
                }
            }

            // reservoir rects
            for(int k = 0; k <= 11; k++)
            {
                if(k == 0 || k == 11)
                {
                    rect.setSize(Vector2f(size_rect.x, size_rect.y/2));
                    k == 0 ? rect.setPosition(540, 0) : rect.setPosition(540, 510);
                    rect.setFillColor(Color(204, 255, 204));
                }
                else
                {
                    rect.setSize(Vector2f(size_rect.x, size_rect.y-12));
                    k > 5 ? rect.setFillColor(Color(255, 255, 255)) : rect.setFillColor(Color(224, 224, 224));
                    rect.setPosition(540, size_rect.x/2 + (k-1) * (distance-12));
                }

                window.draw(rect);

                if(k == 0)
                {
                    text_reservoir.setString("RSVR");
                    text_reservoir.setCharacterSize(20);
                    text_reservoir.setFont(font);
                    text_reservoir.setFillColor(Color::Black);
                    text_reservoir.setStyle(Text::Bold);
                    text_reservoir.setPosition(541, 5);

                    window.draw(text_reservoir);
                }
            }
        }
        else
        {
            cout << "failed to load font file" << endl;
        }

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
