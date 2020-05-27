#include "chess.h"
#include <windows.h>
// #define BOARD_SIZE 64

Chess chess; // global object call
bool valid_test = true; // global variable call

int main()
{
	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest;

	// Create 8x8 default board
	boardInit(chess);
	vector<Piece*> board = chess.getBoard();

	// userEnded(turn);

    while(!chess.getCheckmate()) //while the end of file is NOT reached or game is not finished
    {	
    	cout << endl << "Enter a source AND destination square in [0, 63]: ";
        cin >> src >> dest;
        cout << src << " " << dest << endl;
        chess.makeMove(src, dest);
    }
    
	return 0;
}

/* TODO: 
	1. avoid castling through check
	2. En-passent
	3. Double Checks
	4. check, checkmate, stalemate
*/

		