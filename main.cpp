#include "chess.h"

// #define BOARD_SIZE 64

int main()
{
	// bool valid = false;

	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest;

	// Create 8x8 default board
	boardInit(chess);
	vector<Piece*> board = chess.getBoard();

	// userEnded(turn);

	
	// insert value to the end
	ifstream myfile ("test_cases/castleKingSide.txt"); //opening the file.
    if (myfile.is_open()) //if the file is open
    {
        while (! myfile.eof()) //while the end of file is NOT reached
        {	
        	cout << endl << "Enter a source AND destination square in [0, 63]: ";
            myfile >> src >> dest;
            cout << src << " " << dest << endl;
            chess.makeMove(src, dest);
        }
        myfile.close(); //closing the file
    }
    else
    {
		cout << "Unable to open file"; //if the file is not open output
    	exit(1);
    }
	

	// updatedBoardStatus(board, board[src], turn, valid);
	
	return 0;
}

/* TODO: 
	1. avoid castling through check
	2. En-passent
	3. Double Checks
	4. stalemate
*/

		