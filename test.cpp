/****************************************************************************************************/  
/*									Title:           test.cpp										*/
/*									Author:          Lior Bragilevsky								*/
/*									Related Files:   chess.h										*/
/*									Project:         chessCAMO										*/
/*									Version:         1.0											*/
/*									Last Revision:   June 6th, 2020									*/
/****************************************************************************************************/ 

/*
This implementation file is meant to be used as a test case checker to improve efficiency in debugging.
The algorithm reads in text files that outline a given board position and feeds in the moves presented
in these files one-by-one until the end of each file. At the end, the algorithm compares the expected
FEN string value to the one obtained by making the corresponding moves. If these FEN strings are 
identical then the algorithm increments the number of test cases passed, otherwise it increments the
number of test cases failed. These results are summarized at the end, when all the files are read and
processed, and if a "Failed Test Case" Summary is provided for each test case that failed.

Simply run "mingw32-make all_test && test" on a Windows machine to start the test checking script.

To include an additional test case:
	1) A user can set-up a position in common chess sites like lichess (https://lichess.org/editor)
	   and extract the FEN at the bottom of the board;
	2) The user then makes a text file and pastes this FEN at the top, making sure to remove the last
	   6 characters. E.g. rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1, becomes
	   rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq;
	3) On the next line, the user enters the corresponding source ('src') and destination ('dest') 
	   coordinates for a given move. E.g. 52 36 (note the space) means moving from square 52 to 36;
	4) Rinse and repeat step 3) until the position provided in step 1) is met.
	   Make sure to not end on a blank line to avoid unnecessary warnings from the algorithm;
	5) Name the file a meaningful name according to what is being tested. 
	   E.g. to check if a given piece (Queen) delivers a checkmate -> "queenCheckmate.txt";
	6) Place test case files in a folder named "./test_cases/" (used by the algorithm).

Note:
	- There is no draw or resign functionality here since this is meant for quick testing.
	- If an invalid move is made, the board is printed to allow debugging of the board state.
	- Test cases 12 to 14 require user input to decide the promotion. To avoid errors please enter
	  'q' (or 'Q'), 'n' (or 'N'), and 'b' (or 'B'), respectively. The algorithm will promote to 
	  appropriate piece color based on player's current move, regardless of the input's case.        
*/

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO; 

Chess chess; // global object call

/****************************** Function Declarations ******************************/

// Description:    Converts a given board position into a FEN
//                 (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) string representation
// Pre-condition:  Start with an initialized board vector
// Post-condition: Forms the FEN string based on piece type and common FEN making rules  
string boardFenConverter(const vector<Piece*> & board);

// Description:    Decides what type of character to append to the formed FEN string
// Pre-condition:  'fen'         - FEN string is made (can be empty)
//                 'empty_count' - Number of empty squares found in the current row
//                 'next_char'   - Character to append to the FEN, must be one of P, N, B, Q, K (upper case)
//				   'isWhite'     - true if the passed piece is white, false otherwise (piece case in FEN)
// Post-condition: Appends 'next_char' to 'fen' and returns updated variables by reference 
void appendFEN(string & fen, int & empty_count, char next_char, bool isWhite);
   

int main()
{
	// 'src' -> coordinate of to-be-moved piece, 'dest' -> coordinate of it's final location
	// coordinates are in [0, 63] -> 0 is top left, 63 if bottom right
	int src, dest;

	// can make a separate folder for unit testing or use test_cases/*.txt for all test cases made by author
	string path = "test_cases/*.txt"; 

	char filename[1000], text[100];
	int file_num = 0, num_failed = 0;

	vector<string> failed_tests, fen_obtained, fen_expected;
	vector<int> test_case_num;

	string fen_expected_input;

	// for changing console text color (makes debugging easier)
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, DEFAULT);
	
	// iterate through the files in the folder "./test_cases/" (same directory as project is located in)
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path.c_str(), &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE){cout << "No files found" << endl;	exit(0);}
	else
	{
		do
		{
			sprintf(text, "\n\n===================== TEST CASE %i ====================\n\n", file_num+1);
			chessCAMO::printMessage(text, YELLOW);

			// make sure the board is empty prior to adding pieces AND all game ending conditions are reset
			Chess reset;
			chess = reset;

			// Create 8x8 default board
			chessCAMO::boardInit();

			// form the path to the current test case and open the file
			sprintf(filename,"test_cases/%s", FindFileData.cFileName);
			ifstream myfile(filename); 

		    if(myfile.is_open()) //if the file is open
		    {
		    	// read in the expected FEN (this is at the top of each test case file)
		    	getline(myfile, fen_expected_input);
		    	fen_expected.push_back(fen_expected_input);

		    	// read in the moves of the given test case file one line at a time
		    	// while the end of file is NOT reached and game is NOT finished (checkmate, stalemate, draw, resign)
		        while(!myfile.eof() && !chess.getCheckmate() && !chess.getStalemate()) 
		        {	
		        	chessCAMO::printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
		            myfile >> src >> dest;
		            cout << src << " " << dest << endl;
		            chess.makeMove(src, dest);
		        }
		        myfile.close(); //closing the file
		    }
		    else // test case file failed to open or doesn't exist
		    {
				cout << "Unable to open file"; 
		    	exit(1);
		    }
			
			// convert the final board position from a given test case file into a FEN string 
		    fen_obtained.push_back(boardFenConverter(chess.getBoard()));

		    if(fen_obtained[file_num] != fen_expected[file_num]) // FEN value are not equal
		    {
		    	failed_tests.push_back(filename); // record filename of failed case
		    	test_case_num.push_back(file_num); // record test case number
				num_failed++;
		    }

		   	file_num++;
		} while(FindNextFile(hFind, &FindFileData));
	} 
	FindClose(hFind); // close the directory (end the test case looping)

	/******************** Test Case Summary ********************/
	chessCAMO::printMessage("\n\n\nTest Case Summary\n", YELLOW);

	sprintf(text,"Passed: %i/%i", file_num-num_failed, file_num);
	chessCAMO::printMessage(text, GREEN);

	cout << " | ";

	sprintf(text,"Failed: %i/%i\n\n", num_failed, file_num);
	chessCAMO::printMessage(text, RED);

	if(!failed_tests.empty())
	{
		chessCAMO::printMessage("Failed Cases:\n", YELLOW);

		// iterate through the failed test cases and print their number and file name for easy debugging
		// Also provides the expected and obtained FEN strings to give an idea of what is wrong.
		vector<string>::iterator itr;
		int test_case;
		for(itr = failed_tests.begin(); itr != failed_tests.end(); itr++)
		{
			test_case = test_case_num[itr - failed_tests.begin()];
			cout << *itr;

			sprintf(text, " (Test Case %i)\n", test_case+1);
			chessCAMO::printMessage(text, YELLOW);

			sprintf(text, "Expected FEN: %s\n", fen_expected[test_case].c_str());
			chessCAMO::printMessage(text, GREEN);

			sprintf(text, "Obtained FEN: %s\n", fen_obtained[test_case].c_str());
			chessCAMO::printMessage(text, RED);
		}
	}
	
	return 0;
}		

/****************************** Function Definitions ******************************/

// Description:    Converts a given board position into a FEN
//                 (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) string representation
// Pre-condition:  Start with an initialized board vector
// Post-condition: Forms the FEN string based on piece type and common FEN making rules    
string boardFenConverter(const vector<Piece*> & board)
{
	int elem_count = 0, empty_count = 0;
	string fen;
	char temp[15];

	for(auto elem : board)
	{
		// piece square handling
		if(elem->isPawn())
			appendFEN(fen, empty_count, 'P', elem->isPieceWhite());
		else if(elem->isKnight())
			appendFEN(fen, empty_count, 'N', elem->isPieceWhite());
		else if(elem->isBishop())
			appendFEN(fen, empty_count, 'B', elem->isPieceWhite());
		else if(elem->isRook())
			appendFEN(fen, empty_count, 'R', elem->isPieceWhite());
		else if(elem->isQueen())
			appendFEN(fen, empty_count, 'Q', elem->isPieceWhite());
		else if(elem->isKing())
			appendFEN(fen, empty_count, 'K', elem->isPieceWhite());
		else{empty_count++;} // elem->isEmpty() -> don't append anything to the FEN string

		// empty square handling (must count number of empty square before and after a figure in a given row)
		// E.g. _ _ _ _ P _ _ _ = 4P3
		if(empty_count == 8 || (elem_count % 8 == 7 && empty_count != 0))
		{
			sprintf(temp, "%i", empty_count);
			fen.append(temp);
			empty_count = 0;
		}

		// end of row handling
		if(elem_count % 8 == 7 && elem_count != 63){fen.append("/");}

		elem_count++;
	}

	// player turn handling
	fen.append(chess.getTurn() == WHITE ? " w " : " b ");

	// castle handling (for both sides)
	/***** White player *****/
	if(board[63]->isRook() && board[60]->isKing() && board[60]->isSameColor(63) && !board[60]->getPieceMoveInfo() && !board[63]->getPieceMoveInfo())
		fen.append("K");
	if(board[56]->isRook() && board[60]->isKing() && board[60]->isSameColor(56) && !board[56]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo())
		fen.append("Q");

	/***** Black player *****/
	if(board[7]->isRook() && board[4]->isKing() && board[4]->isSameColor(7) && !board[4]->getPieceMoveInfo() && !board[7]->getPieceMoveInfo())
		fen.append("k");
	if(board[0]->isRook() && board[4]->isKing() && board[4]->isSameColor(0) && !board[0]->getPieceMoveInfo() && !board[4]->getPieceMoveInfo())
		fen.append("q");

	return fen;
}

// Description:    Decides what type of character to append to the formed FEN string
// Pre-condition:  'fen'         - FEN string is made (can be empty)
//                 'empty_count' - Number of empty squares found in the current row
//                 'next_char'   - Character to append to the FEN, must be one of P, N, B, Q, K (upper case)
//				   'isWhite'     - true if the passed piece is white, false otherwise (to determine case of piece in FEN)
// Post-condition: Appends 'next_char' to 'fen' and returns updated variables by reference   
void appendFEN(string & fen, int & empty_count, char next_char, bool isWhite)
{
	// accumulated empty squares leading up to a piece in a given row?
	// append this number to the FEN string and reset the empty squares counter for the row
	if(empty_count > 0)
	{
		char temp[15];
		sprintf(temp, "%i", empty_count);
		fen.append(temp);
		empty_count = 0; // found a piece or went to new line, so empty square counter cannot be > 0
	}

	// White piece -> upper case letter, black piece -> lower case letter
	isWhite ? fen.push_back((char) next_char) : fen.push_back((char) tolower(next_char));
}