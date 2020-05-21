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

	int i = 19;
	for(auto val : board[i].bitify(Diag_R, board))
	{
		cout << val << " ";
	}
	cout << endl;
	for(auto val : board[i].bitify(Diag_L, board))
	{
		cout << val << " ";
	}

	cout << endl;
	for(auto val : board[i].bitify(Row, board))
	{
		cout << val << " ";
	}
	cout << endl;
	for(auto val : board[i].bitify(Col, board))
	{
		cout << val << " ";
	}

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