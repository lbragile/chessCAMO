#include "chess.h"

Chess chess; // global object call

int main()
{
	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	string src, dest;

	// Create 8x8 default board
	boardInit(chess);

	// userEnded(turn);

    while(!chess.getCheckmate() && !chess.getStalemate())
    {	
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), PINK);
		cout << "\nEnter a source AND destination square in [0, 63]: ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), DEFAULT);
        cin >> src >> dest;
        chess.makeMove(src, dest);
    }

	return 0;
}

/* TODO: 
	1. Double Checks
	2. Three move repetition (draw) & 50 move rule
	3. Draw offer / Resign

	4. GUI
	5. Input as PGN rather than two ints (template)
*/

		