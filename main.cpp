#include "switcher.h"

// extern variable
stack<Chess> checkStack; // needed to determine if a given player's king is in check

// #define BOARD_SIZE 64

int main()
{
	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest, turn = 1;

	// 8x8 board stored as a vector of objects
	vector<Chess> board{64}; 
	initBoard(board);

	while(true) // change to while game is not over! -> need checkmate, resign, draw/stalemate options for this
	{
		cout << endl << "Enter a source AND destination square in [0, 63]: ";
		cin >> src >> dest;
		board[src].makeMove(dest, board, turn, false);
	}
	
	return 0;
}

/* TODO: 
	1. Check detection, checkmate, stalemate
	2. castling, avoid castling through check
	3. En-passent
	4. Resign / Draw offers
*/