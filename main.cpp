#include "chess.h"

// extern variables
stack<Chess> checkStack; // needed to determine if a given player's king is in check
bool checkmate = false, stalemate = false, check = false;

// #define BOARD_SIZE 64

int main()
{
	bool valid = false;

	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest, turn = 1;

	// 8x8 board stored as a vector of objects
	vector<Chess> board{64}; 
	initBoard(board);

	while(!checkmate && !stalemate) // change to while game is not over! -> need checkmate, resign, draw/stalemate options for this
	{
		userEnded(turn);

		cout << endl << "Enter a source AND destination square in [0, 63]: ";
		cin >> src >> dest;
		board[src].makeMove(dest, board, turn, valid);

		updatedBoardStatus(board, board[src], turn, valid);
	}
	
	return 0;
}

/* TODO: 
	1. avoid castling through check
	2. En-passent
	3. Double Checks
	4. stalemate
*/