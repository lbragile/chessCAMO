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
        pieces[i].setPosition(shape.width + shape.width*(i%8), shape.height + shape.height*(i/8));
        i++;
    }

}
int main()
{
    // cout.setstate(std::ios_base::failbit); // surpress output

    Chess *chess = new Chess;

    // Create 8x8 default board
    chess->boardInit();

    stack<vector<Piece*>> board_positions;
    board_positions.push(chess->getBoard());

    RenderWindow window(VideoMode(600,600), "chessCAMO");

    // board
    Texture board;
    board.loadFromFile("images/board.png"); 
    Sprite sBoard(board); 

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

    drawPieces(pieces, pieceType, chess->getBoard());
    // sBoard.setScale(600/721.0, 600/721.0);

    Vector2f size_rect(60.0, 60.0);
    Vector2f lr_rect(size_rect.x/2, size_rect.y);
    Vector2f tb_rect(size_rect.x, size_rect.y/2);
    RectangleShape rect(size_rect);
    const int numRows = 10; //number of rows
    const int numCols = 10; //number of columns
    const int distance = 60; //distance between squuares
    Color color_dark(211, 211, 211, 255); // color of dark squares
    bool clicked = false;

    while(window.isOpen())
    {   
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;

        Event e;
        while (window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();   

            if(e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Space && board_positions.size() > 1)
            {
                board_positions.pop();
                chess->setBoard(board_positions.top());
                chess->setTurn(chess->getTurn() == WHITE ? BLACK : WHITE);
                drawPieces(pieces, pieceType, chess->getBoard());
                break;
            }

            if(e.type == Event::MouseButtonPressed)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    clicked = true;
                    for(iter = pieces.begin(); iter != pieces.end(); iter++)
                    {
                        if(iter->getGlobalBounds().contains(pos.x, pos.y))
                            src = iter - pieces.begin();
                    }
                }
            }
                
            if(e.type == Event::MouseButtonReleased)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    clicked = false;
                    cout << pos.x << " " << pos.y << endl;
                    cout << int((pos.x / size_rect.x) - 1) << " " << ((pos.y / size_rect.y) - 1) << endl;
                    dest = int((pos.x / size_rect.x) - 1) +  8*int((pos.y / size_rect.y) - 1) ;
                }

                // here means that you pressed and released the mousebutton,
                // so can make a move
                 cout << src << " " << dest << endl;
                chess->makeMove(src, dest, cin);
                drawPieces(pieces, pieceType, chess->getBoard());
                board_positions.push(chess->getBoard());         
            }   

            if(clicked)
                pieces[src].setPosition( pos.x - size_rect.x/2, pos.y - size_rect.y/2 ); 
        }

        window.clear();

        // board
        for(int i=1; i < numRows-1; i++)
        {
            for(int j=1; j < numCols-1; j++)
            {
                rect.setPosition(j*distance, i*distance);
                rect.setFillColor((i+j)%2 != 0 ? color_dark : Color::White);
                window.draw(rect);  
            }
        }

        // pieces
        for(const auto & piece : pieces)
            window.draw(piece);

        window.display();
    }

    delete chess; // destroy the dynamic object to free its memory allocation
    cout.clear();

    return 0; 
}
