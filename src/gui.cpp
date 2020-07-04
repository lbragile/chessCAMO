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

    RenderWindow window(VideoMode(780, 600), "chessCAMO", Style::Titlebar | Style::Close);

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

    drawPieces(pieces, pieceType, chess.getBoard());

    Vector2f size_rect(60.0, 60.0); // main squares
    Vector2f lr_rect(size_rect.x / 2, size_rect.y); // left and right columns
    Vector2f tb_rect(size_rect.x, size_rect.y / 2); // top and bottom rows

    Vector2f size_side(240.0, 540.0); // side panel
    Vector2f size_bottom(780.0, 60.0); // bottom panel

    RectangleShape rect(size_rect);
    RectangleShape side_rect(size_side);
    RectangleShape bottom_rect(size_bottom);

    Text text, text_side, text_bottom;
    Font font;
    const int numRows = 10;
    const int numCols = 10;
    const int distance = 60; //distance between squares
    Color color_dark(211, 211, 211, 255);
    Color color_yellow(255, 255, 153, 255);
    Color color_orange(255, 204, 153, 255);
    Color color_red(255, 204, 204, 255);
    bool clicked = false, move = false;
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

            if(!chess.getCheckmate() && !chess.getStalemate())
            {
                if(e.type == Event::KeyPressed)
                {
                    streambuf *coutbuf = cout.rdbuf();
                    cout.rdbuf(messageOut.rdbuf());

                    if(e.key.code == Keyboard::U) 
                    {
                        // decrement move counter by 1 since an illegal move was made 
                        chess.setNumMoves(chess.getNumMoves()-1);

                        // restore previous object
                        chessCAMO::restoreObject(chess.getNumMoves(), chess);

                        drawPieces(pieces, pieceType, chess.getBoard());
                    }

                    else if(e.key.code == Keyboard::Escape)
                    {
                        string message = chess.getTurn() == WHITE ? "White resigned -> Black wins\n"
                                                                  : "Black resigned -> White wins\n";
                        cout << message;
                        chess.setCheckmate(true);
                    }

                    // else if(e.key.code == Keyboard::D)
                    // {
                    //     ofstream outFile(filename, ios::trunc);
                    //     outFile << 'd' << endl;
                    //     outFile << 'n' << endl;
                    //     outFile.close();

                    //     ifstream inFile(filename);
                    //     chessCAMO::drawOrResign(false, chess, inFile);
                    //     inFile.close();
                    // }

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

                    cout.rdbuf(coutbuf);
                }

                if(e.type == Event::MouseButtonPressed)
                {
                    if(e.mouseButton.button == Mouse::Left && 30 < pos.x && pos.x < 510 && 30 < pos.y && pos.y < 510)
                    {
                        clicked = true;
                        for(iter = pieces.begin(); iter != pieces.end(); iter++)
                            if(iter->getGlobalBounds().contains(pos.x, pos.y))
                                src = iter - pieces.begin();

                        if(0 > src || 63 < src)
                        {
                            move = false;
                        }
                        else
                        {
                            move = true;
                            getLegalMoves(legalMoves, src, chess); // updates legalMoves by reference
                        }
                    }
                }

                if(e.type == Event::MouseButtonReleased)
                {
                    if(e.mouseButton.button == Mouse::Left && move)
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
            // // background
            // bottom_rect.setPosition(0, 540);
            // bottom_rect.setFillColor(Color(204, 255, 255));

            // side_rect.setPosition(540, 0);
            // side_rect.setFillColor(Color(204, 255, 204));

            // window.draw(side_rect);
            // window.draw(bottom_rect);

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
            // warning and status update messages are always on line 21
            for(int i = 0; i < 21; i++)
               getline(messageIn, second_message, '\n');
            messageIn.close();

            // only print warning if it is one of the following
            string possible_warnings[] = {"White won by Checkmate!", "Black won by Checkmate!",
                                          "White has no moves -> Game is Drawn!", "Black has no moves -> Game is Drawn!", 
                                          "Game drawn by agreement", "Draw rejected. Game continues...",
                                          "You are in check! Try again...", "You are in double check! Try again...",
                                          "Check!", "Double Check!"};
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

            text_side.setPosition(555, 50);
            text_side.setString("To promote, first type \none of the following \ncharacters, then make \nthe move:\n\n"
                                "1. Q (or q)\n\n"
                                "2. R (or r)\n\n"
                                "3. N (or n)\n\n"
                                "4. B (or b)\n\n");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(655, 165);
            text_side.setString("=> Queen\n\n"
                                "=> Rook\n\n"
                                "=> Knight\n\n"
                                "=> Bishop");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(555, 400);
            text_side.setString("Resign:\n\n"
                                "Offer draw:\n\n"
                                "Undo Move:\n\n");
            text_side.setStyle(Text::Regular);
            window.draw(text_side);

            text_side.setPosition(680, 400);
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
        }
        else
        {
            cout << "failed to load font file" << endl;
        }

        // pieces
        for (const auto &piece : pieces)
            window.draw(piece);

        window.display();
    }

    cout.clear();

    return 0;
}
