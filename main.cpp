#include "switcher.h"
#include <vector>

using namespace std;

#define BOARD_SIZE 64

int main()
{
	unsigned int dest;
	bool valid = false;
	vector<Switcher> board = initBoard(BOARD_SIZE);
	printBoard(board);

	// cout << board[63].isValidMove(board[63].getPieceSquare(), 46);

	while(!valid)
	{
		cout << endl << "Enter destination square in [0, 63]: ";
		cin >> dest;
		if(0 <= dest && dest <= 63)
			valid = board[52].makeMove(board, dest);
		else
			cout << endl << "Invalid move, try again!" << endl;
	}

	printBoard(board);

	return 0;
}