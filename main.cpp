#include "switcher.h"
#include <vector>

using namespace std;

#define BOARD_SIZE 64

int main()
{
	int src, dest;
	bool valid;
	vector<Chess> board = initBoard(BOARD_SIZE);
	printBoard(board);


	while(true)
	{
		valid = false;
		cout << endl << "Enter a source AND destination square in [0, 63]: ";
		cin >> src >> dest;
		if(0 <= dest && dest <= 63 && (dest-src) != 0)
			valid = board[src].makeMove(board, dest);
			
		if(valid)
			printBoard(board);
		else
			cout << endl << "Invalid move, try again!" << endl;
	}
	
	return 0;
}