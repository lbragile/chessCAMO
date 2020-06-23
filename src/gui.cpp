#include <SFML/Graphics.hpp>
#include <vector>
#include "chess.h"
#include "chess.cpp"

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
        pieces[i].setPosition((shape.width-10)+80*(i%8), (shape.height-10)+80*(i/8));
        i++;
    }

}
int main()
{
    // cout.setstate(std::ios_base::failbit); // surpress output

    Chess *chess = new Chess;

    // Create 8x8 default board
    chess->boardInit();

    RenderWindow window(VideoMode(721,721), "chessCAMO");

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

    while(window.isOpen())
    {   
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;
        FloatRect shape;

        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                window.close();   

            if (e.type == Event::MouseButtonPressed && e.mouseButton.button == Mouse::Left)
            {
                
                for(iter = pieces.begin(); iter != pieces.end(); iter++)
                {
                    shape = iter->getGlobalBounds();
                    if(shape.contains(pos.x, pos.y))
                    {
                        src = iter - pieces.begin();
                        break;
                    }
                }
            }

            else if (e.type == Event::MouseButtonReleased && e.mouseButton.button == Mouse::Left)
            {
                  for(iter = pieces.begin(); iter != pieces.end(); iter++)
                {
                    shape = iter->getGlobalBounds();
                    if(shape.contains(pos.x, pos.y))
                    {
                        dest = iter - pieces.begin();
                        break;
                    }
                }

                // here means that you pressed and released the mousebutton,
                // so can make a move
                chess->makeMove(src, dest, cin);
                drawPieces(pieces, pieceType, chess->getBoard());         
            }    
        }

        window.clear();

        window.draw(sBoard);
        for(const auto & piece : pieces)
            window.draw(piece);

        window.display();
    }

    delete chess; // destroy the dynamic object to free its memory allocation
    cout.clear();

    return 0; 
}
