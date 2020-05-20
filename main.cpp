#include "switcher.h"
#include <vector>

using namespace std;

#define BOARD_SIZE 64

int main()
{
	int src, dest;
	bool valid = false;
	vector<Switcher> board = initBoard(BOARD_SIZE);
	printBoard(board);

	// cout << board[63].isValidMove(board[63].getPieceSquare(), 46);
	while(true)
	{
		cout << endl << "Enter a source AND destination square in [0, 63]: ";
		cin >> src >> dest;
		if(0 <= dest && dest <= 63)
			valid = board[src].makeMove(board, dest);
			
		if(valid)
			printBoard(board);
		else
			cout << endl << "Invalid move, try again!" << endl;

		if(src == -100)
			break;
	}
	
	return 0;
}