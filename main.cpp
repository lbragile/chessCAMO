#include "chess.h"

using namespace std;

Chess chess; // global object call

int main()
{
	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest;

	// Create 8x8 default board
	chess.boardInit();

	// userEnded(turn);

    while(!chess.getCheckmate() && !chess.getStalemate())
    {	
    	chess.printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
        cin >> src >> dest;
        std::system("cls"); // clear the screen
        chess.makeMove(src, dest);
    }

	return 0;
}

/* TODO: 
	1. Three move repetition (draw) & 50 move rule
	2. Draw offer / Resign

	3. GUI
	4. Input as PGN rather than two ints (template)
*/

		