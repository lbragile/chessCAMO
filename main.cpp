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
		valid = board[63].makeMove(board, dest);
	}

	printBoard(board);

	return 0;
}