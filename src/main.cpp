/**
 * \page main Main Implementation File Description (Allow User to Play via a Windows Console)
 * 
 * <b>Title</b><br>
 * <span>&emsp;&emsp;&emsp;main.cpp</span>
 * \author Lior Bragilevsky<br>
 * 
 * <b>Related</b><br>
 * <span>&emsp;&emsp;&emsp;chess.h</span><br> 
 * 
 * <b>Project</b><br>
 * <span>&emsp;&emsp;&emsp;chessCAMO</span><br>
 * \version \version_num
 * \date \today
 *
 * This implementation file is meant to be used by avid chess players. Once the program is running,
 * the player inputs a source ('src') coordinate and destination ('dest') coordinate to make a move.
 * If the user enters an invalid move, an appropriate error message is displayed. At any given moment,
 * a player can decide to either continue, offer a draw, resign, or undo the move.
 *
 * Simply run <b>mingw32-make all_main && main</b> on a Windows machine to start the game.
 *
 * \note
 *   - Currently all standard chess rules are supported, except three move repetition & 50 move rule.
 *   - You can choose to input PGN notation ('e2 E4') rather than coordinates ('52 36'), or a mix of both ('e2 36').   
*/

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

/**
 * @brief      Simulates a chess game between two players.
 *             At any point, the player whose turn it is, can decide whether they want to
 *             continue, offer a draw, or resign. Depending on the response from the
 *             opponent (if required), the game either continue or end.
 *             If game continues, the player must choose a turn to make by selecting the 
 *             source and destination squares corresponding to the piece they would like to
 *             move. Error checking exists to prevent invalid input. The console screen
 *             refreshes after a valid move is made and the board position is updated and
 *             printed to illustrate the current position. Relevant warning messages are 
 *             displayed and the game ends when the engine detects that the game is over.
 *
 * @return     0 if program exited successfully
 */
int main()
{
    // 'src' -> coordinate of to-be-moved piece, 'dest' -> coordinate of it's final location
    // Coordinates are in [A1, H8] -> A1 is bottom left, H8 is top right
    string src, dest;

    // create the object dynamically to control when it is destroyed
    Chess chess;
    
    // Create 8x8 default board
    chess.boardInit();

    // read in the moves provided by the players (one at a time)
    // while the game is NOT finished (checkmate, stalemate, draw, resign)
    while(!chess.getCheckmate() && !chess.getStalemate())
    {   
        chessCAMO::printMessage("\nFor reservoir use source values:\n"
                                "|  Q/q  |  R/r |  O/o   |  N/n   |  P/p |\n"
                                "| Queen | Rook | Bishop | Knight | Pawn |", DEFAULT);
        chessCAMO::printMessage("\nEnter a source AND destination square in [A1, H8]: ", PINK);
        cin >> src >> dest;

        // makes the corresponding move and updates the board state on the console after clearing it
        chessCAMO::clearScreen(true);
        chess.makeMove(chessCAMO::preProcessInput(src), chessCAMO::preProcessInput(dest), cin); // return value isn't used

        // prevent asking again after game is over
        if(!chess.getCheckmate() && !chess.getStalemate())
        {
            chessCAMO::drawOrResign(true, chess, cin);
            
            // drawOrResign can set the checkmate flag to true if player chooses to resign or draw
            // so if this happens break out of the while loop
            if(chess.getCheckmate())
                break;
        }
    }

    return 0;
}       