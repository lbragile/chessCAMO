/****************************************************************************************************/
/*                                  Title:           main.cpp                                       */
/*                                  Author:          Lior Bragilevsky                               */
/*                                  Related Files:   chess.h                                        */
/*                                  Project:         chessCAMO                                      */
/*                                  Version:         1.0                                            */
/*                                  Last Revision:   June 5th, 2020                                 */
/****************************************************************************************************/

/*
This implementation file is meant to be used by avid chess players. Once the program is running,
the player inputs a source ('src') coordinate and destination ('dest') coordinate to make a move.
If the user enters an invalid move, an appropriate error message is displayed. At any given moment,
a player can decide to either continue, offer a draw, or resign.

Simply run "mingw32-make all_main && main" on a Windows machine to start the game.

Note:
    - Currently all standard chess rules are supported, except three move repetition & 50 move rule.
    - There is no undo board representation (yet!).
    - You can choose to input PGN notation ('e2 e4') rather than coordinates ('52 36'), but you must be
      consistant at each input. That is, you cannot input mixed notations such as 'e2 36'.
    - GUI is in the works, stay tuned!      
*/

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

int main()
{
    // 'src' -> coordinate of to-be-moved piece, 'dest' -> coordinate of it's final location
    // coordinates are in [0, 63] -> 0 is top left, 63 if bottom right
    int src, dest;

    // Create 8x8 default board
    chessCAMO::boardInit();

    // read in the moves provided by the players (one at a time)
    // while the game is NOT finished (checkmate, stalemate, draw, resign)
    while(!chess.getCheckmate() && !chess.getStalemate())
    {   
        chessCAMO::printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
        cin >> src >> dest;

        // input error checking
        while(cin.fail())
        {
            chessCAMO::printMessage("\nInput type is invalid... try again!\n", YELLOW);
            cin.clear(); // clear error flag of cin
            cin.ignore(100,'\n'); // ignore the first 100 characters until '\n' is reached
            chessCAMO::printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
            cin >> src >> dest;
        }

        // makes the corresponding move and updates the board state on the console after clearing it
        std::system("cls");
        chess.makeMove(src, dest); 
        chessCAMO::drawOrResign();
    }

    return 0;
}       