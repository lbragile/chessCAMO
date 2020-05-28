#include "chess.h"
#include <windows.h>
#include <string>

#define GREEN 10
#define RED 12
#define YELLOW 14
#define DEFAULT 15

Chess chess; // global object call

string boardFenConverter(const vector<Piece*> & board)
{
	char temp[15];
	string fen;

	int elem_count = 0, empty_count = 0;

	for(auto elem : board)
	{
		sprintf(temp, "%i", empty_count);
		if(elem->isPawn()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("P") : fen.append("p"); empty_count=0;}
		else if(elem->isKnight()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("N") : fen.append("n"); empty_count=0;}
		else if(elem->isBishop()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("B") : fen.append("b"); empty_count=0;}
		else if(elem->isRook()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("R") : fen.append("r"); empty_count=0;}
		else if(elem->isQueen()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("Q") : fen.append("q"); empty_count=0;}
		else if(elem->isKing()){if(empty_count > 0){fen.append(temp);} elem->isPieceWhite() ? fen.append("K") : fen.append("k"); empty_count=0;}
		else{empty_count++;} // elem->isEmpty()

		if(empty_count == 8 || (elem_count % 8 == 7 && empty_count != 0)){sprintf(temp, "%i", empty_count); fen.append(temp); empty_count = 0;}

		if(elem_count % 8 == 7 && elem_count != 63){fen.append("/");}
		elem_count++;
	}
	fen.append(chess.getTurn() == 2 ? " w " : " b ");

	// castling
	if(board[63]->isRook() && board[60]->isKing() && board[60]->isSameColor(60, 63) && !board[60]->getPieceMoveInfo() && !board[63]->getPieceMoveInfo()){fen.append("K");}
	if(board[56]->isRook() && board[60]->isKing() && board[60]->isSameColor(56, 60) && !board[56]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo()){fen.append("Q");}
	if(board[7]->isRook() && board[4]->isKing() && board[4]->isSameColor(4, 7) && !board[4]->getPieceMoveInfo() && !board[7]->getPieceMoveInfo()){fen.append("k");}
	if(board[0]->isRook() && board[4]->isKing() && board[4]->isSameColor(0, 4) && !board[0]->getPieceMoveInfo() && !board[4]->getPieceMoveInfo()){fen.append("q");}

	return fen;
}

int main()
{
	char filename[1000];
	int file_num = 0, num_failed = 0, src, dest;
	vector<string> failed_tests, fen_obtained, fen_expected;
	vector<int> test_case_num;
	string fen_expected_input;

	HANDLE  hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, DEFAULT);
	
	WIN32_FIND_DATA FindFileData;
	string absolute_path = "C:/Users/lbrag/Desktop/c++/personal/test_cases/*.txt";
	HANDLE hFind = FindFirstFile(absolute_path.c_str(), &FindFileData);
	if(hFind == INVALID_HANDLE_VALUE){cout << "No files found" << endl;	exit(0);}
	else
	{
		do
		{
			SetConsoleTextAttribute(hConsole, YELLOW);
			printf("\n\n===================== TEST CASE %i ==================== \n\n", file_num+1);
			SetConsoleTextAttribute(hConsole, DEFAULT);

			Chess reset;
			chess = reset;

			// Create 8x8 default board
			boardInit(chess);

			// userEnded(turn);

			// insert value to the end
			sprintf(filename,"test_cases/%s", FindFileData.cFileName);
			ifstream myfile(filename); //opening the file.
		    if(myfile.is_open()) //if the file is open
		    {
		    	getline(myfile, fen_expected_input);
		    	fen_expected.push_back(fen_expected_input);
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
		    fen_obtained.push_back(boardFenConverter(chess.getBoard()));

		    if(fen_obtained[file_num] != fen_expected[file_num]) // FEN value are not equal
		    {
		    	failed_tests.push_back(filename);
		    	test_case_num.push_back(file_num);
				num_failed++;
		    }

		   	file_num++;
		} while (FindNextFile(hFind, &FindFileData));
	} 
	FindClose(hFind);

	cout << endl << endl << endl;
	SetConsoleTextAttribute(hConsole, YELLOW);
	cout <<"Test Case Summary" << endl;
	SetConsoleTextAttribute(hConsole, GREEN);
	cout << "Passed: " << file_num-num_failed << "/" << file_num;
	SetConsoleTextAttribute(hConsole, DEFAULT);
	cout << " | ";
	SetConsoleTextAttribute(hConsole, RED);
	cout << "Failed: " << num_failed << "/" << file_num << endl;
	cout << endl << endl;

	if(!failed_tests.empty())
	{
		cout << "Failed Cases:" << endl;
		SetConsoleTextAttribute(hConsole, DEFAULT);
		vector<string>::iterator itr;
		int test_case;
		for(itr = failed_tests.begin(); itr != failed_tests.end(); itr++)
		{
			test_case = test_case_num[itr - failed_tests.begin()];
			cout << *itr;
			SetConsoleTextAttribute(hConsole, YELLOW);
			cout << " (Test Case " << test_case+1 << ")" << endl;
			SetConsoleTextAttribute(hConsole, GREEN);
			cout << "Expected FEN: ";
			SetConsoleTextAttribute(hConsole, DEFAULT);
			cout << fen_expected[test_case] << endl;
			SetConsoleTextAttribute(hConsole, RED);
			cout << "Obtained FEN: ";
			SetConsoleTextAttribute(hConsole, DEFAULT);
			cout << fen_obtained[test_case] << endl << endl << endl;
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

		