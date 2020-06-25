#include <SFML/Graphics.hpp>
#include <vector>
#include "chess.h"

using namespace sf;
using namespace std;

void drawPieces(vector<Sprite> &pieces, const vector<Sprite> &pieceType, const vector<Piece *> &board)
{
    FloatRect shape;
    int i = 0;
    for(const auto &elem : board)
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

void getLegalMoves(vector<int> &legalMoves, int src, Chess *chess)
{
    vector<Piece *> board = chess->getBoard();

    for(unsigned int dest = 0; dest < board.size(); dest++)
    {
        if(board[src]->isLegalMove(dest, chess))
        {
            legalMoves.push_back(dest);
        }
    }
}

int main()
{
    Chess *chess = new Chess;

    // Create 8x8 default board
    chess->boardInit();

    stack<vector<Piece *>> board_positions;
    vector<int> squares_prior(64);
    vector<bool> moved_prior(64), enpassant_prior(64);
    vector<int> legalMoves;

    board_positions.push(chess->getBoard());

    RenderWindow window(VideoMode(540, 540), "chessCAMO", Style::Titlebar | Style::Close);

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

    Vector2f size_rect(60.0, 60.0);
    Vector2f lr_rect(size_rect.x / 2, size_rect.y);
    Vector2f tb_rect(size_rect.x, size_rect.y / 2);
    RectangleShape rect(size_rect);
    Text text;
    Font font;
    const int numRows = 10;
    const int numCols = 10;
    const int distance = 60; //distance between squuares
    Color color_dark(211, 211, 211, 255);
    Color color_yellow(255, 255, 153, 255);
    Color color_orange(255, 204, 153, 255);
    Color color_red(255, 204, 204, 255);
    bool clicked = false, getMoves = true;

    while(window.isOpen())
    {
        Vector2i pos = Mouse::getPosition(window);
        int src, dest;
        vector<Sprite>::const_iterator iter;

        Event e;
        while(window.pollEvent(e))
        {
            if(e.type == Event::Closed)
                window.close();

            if(e.type == sf::Event::KeyPressed)
            {
                if(e.key.code == sf::Keyboard::Space && board_positions.size() > 1)
                {
                    board_positions.pop();
                    chess->setTurn(chess->getTurn() == WHITE ? BLACK : WHITE);
                    chess->setBoard(board_positions.top());

                    chess->storeOrRestore(board_positions.top(), squares_prior, moved_prior, enpassant_prior, "restore");
                    drawPieces(pieces, pieceType, chess->getBoard());
                }
            }

            if(e.type == Event::MouseButtonPressed)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    clicked = true;
                    for(iter = pieces.begin(); iter != pieces.end(); iter++)
                    {
                        if(iter->getGlobalBounds().contains(pos.x, pos.y))
                        {
                            src = iter - pieces.begin();
                        }
                    }

                    if(getMoves)
                    {
                        getLegalMoves(legalMoves, src, chess); // updates legalMoves by reference
                        getMoves = false;
                    }
                }
            }

            if(e.type == Event::MouseButtonReleased)
            {
                if(e.mouseButton.button == Mouse::Left)
                {
                    clicked = false;
                    getMoves = true;
                    dest = int((pos.x / size_rect.x) - 0.5) + 8 * int((pos.y / size_rect.y) - 0.5);

                    // here means that you pressed and released the mousebutton,
                    // so can make a move
                    chess->storeOrRestore(board_positions.top(), squares_prior, moved_prior, enpassant_prior, "store");
                    chess->makeMove(src, dest, cin);

                    drawPieces(pieces, pieceType, chess->getBoard());

                    // add new position to the stack
                    if (chess->getBoard() != board_positions.top())
                        board_positions.push(chess->getBoard());

                    // clear all legal moves for next cycle
                    while(!legalMoves.empty())
                        legalMoves.pop_back();
                }
            }

            if(clicked)
                pieces[src].setPosition(pos.x - size_rect.x / 2, pos.y - size_rect.y / 2);
        }

        window.clear();

        // board
        if(font.loadFromFile("font/arial.ttf"))
        {
            text.setCharacterSize(20);
            text.setFont(font);
            text.setFillColor(Color::Black);

            // center the text inside it's bounding box
            FloatRect textRect = text.getLocalBounds();
            text.setOrigin(textRect.left + textRect.width/2.0, textRect.top  + textRect.height/2.0);

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
                        {
                            rect.setPosition(j * distance, i * distance);
                        }
                        else if(i == 0 && j == 9)
                        {
                            rect.setPosition(j * distance - size_rect.x/2, i * distance);
                        }
                        else if(i == 9 && j == 0)
                        {
                            rect.setPosition(j * distance, i * distance - size_rect.y/2);
                        }
                        else if(i == 9 && j == 9)
                        {
                            rect.setPosition(j * distance - size_rect.x/2, i * distance - size_rect.y/2);
                        }

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
                    if(found != legalMoves.end() && chess->getBoard()[src]->getPieceColor() == chess->getTurn() && 
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

    delete chess; // destroy the dynamic object to free its memory allocation
    cout.clear();

    return 0;
}
