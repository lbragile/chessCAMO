#include "chess.h"
#include <windows.h>
// #define BOARD_SIZE 64

Chess chess; // global object call
bool valid_test = true; // global variable call

int main()
{
	char filename[1000];
	int file_num = 1, num_failed = 0;
	vector<string> failed_tests;
	vector<int> test_case_num;

	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 15);
	
	WIN32_FIND_DATA FindFileData;
	string absolute_path = "C:/Users/lbrag/Desktop/c++/personal/test_cases/*.txt";
	HANDLE hFind = FindFirstFile(absolute_path.c_str(), &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE){cout << "No files found" << endl;	exit(0);}
	else
	{
		do
		{
			SetConsoleTextAttribute(hConsole, 14);
			printf("\n\n===================== TEST CASE %i ==================== \n\n", file_num);
			SetConsoleTextAttribute(hConsole, 15);

			Chess reset;
			chess = reset;

			// 'src' -> start square index, 'dest' -> end square index, 'turn' -> 1 (white) or -1 (black)
			int src, dest;

			// Create 8x8 default board
			boardInit(chess);
			vector<Piece*> board = chess.getBoard();

			// userEnded(turn);

			// insert value to the end
			sprintf(filename,"test_cases/%s", FindFileData.cFileName);
			ifstream myfile(filename); //opening the file.
		    if(myfile.is_open()) //if the file is open
		    {
		        while(!myfile.eof() && !chess.getCheckmate()) //while the end of file is NOT reached or game is not finished
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
		    file_num++;
		    if(!valid_test)
		    {
		    	failed_tests.push_back(filename);
		    	test_case_num.push_back(file_num-1);
				num_failed++;
		    }
		} while (FindNextFile(hFind, &FindFileData));
	} 
	FindClose(hFind);

	cout << endl << endl << endl;
	SetConsoleTextAttribute(hConsole, 14);
	cout <<"Test Case Summary" << endl;
	SetConsoleTextAttribute(hConsole, 10);
	cout << "Passed: " << file_num-num_failed << "/" << file_num;
	SetConsoleTextAttribute(hConsole, 15);
	cout << " | ";
	SetConsoleTextAttribute(hConsole, 12);
	cout << "Failed: " << num_failed << "/" << file_num << endl;
	cout << endl << endl;

	if(!failed_tests.empty())
	{
		cout << "Failed Cases:" << endl;
		SetConsoleTextAttribute(hConsole, 15);
		vector<string>::iterator itr;
		for(itr = failed_tests.begin(); itr != failed_tests.end(); itr++)
		{
			cout << *itr;
			SetConsoleTextAttribute(hConsole, 14);
			cout << " (Test Case " << test_case_num[itr - failed_tests.begin()] << ")" << endl;
			SetConsoleTextAttribute(hConsole, 15);
		}
	}
	
	return 0;
}

/* TODO: 
	1. avoid castling through check
	2. En-passent
	3. Double Checks
	4. check, checkmate, stalemate
*/

		