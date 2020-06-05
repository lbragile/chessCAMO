#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

Chess chess; // global object call

int main()
{
	// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
	int src, dest;

	// Create 8x8 default board
	chessCAMO::boardInit();

	// userEnded(turn);

    while(!chess.getCheckmate() && !chess.getStalemate())
    {	
    	chessCAMO::printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
        cin >> src >> dest;

        // input error checking
        while(cin.fail())
        {
	        chessCAMO::printMessage("\nInput type is invalid... try again!\n", YELLOW);
	        cin.clear();
	        cin.ignore(100,'\n');
    		chessCAMO::printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
	        cin >> src >> dest;
	    }

        std::system("cls"); // clear the screen
        chess.makeMove(src, dest);
		chessCAMO::drawOrResign();
    }

	return 0;
}		