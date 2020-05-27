#include "chess.h"
#include <windows.h>
#include <string>

#define GREEN 10
#define RED 12
#define YELLOW 14
#define DEFAULT 15

Chess chess; // global object call
bool valid_test = true; // global variable call

string boardFenConverter(const vector<Piece*> & board, int white_turns, int black_turns)
{
	char temp[15];
	string fen;

	bool w_castle_k = false, w_castle_q = false, b_castle_k = false, b_castle_q = false;
	int elem_count = 0, empty_count = 0;

	for(auto elem : board)
	{
		if(elem->isPawn()){elem->isPieceWhite() ? fen.append("P") : fen.append("p");}
		if(elem->isKnight()){elem->isPieceWhite() ? fen.append("N") : fen.append("n");}
		if(elem->isBishop()){elem->isPieceWhite() ? fen.append("B") : fen.append("b");}
		if(elem->isRook()){elem->isPieceWhite() ? fen.append("R") : fen.append("r");}
		if(elem->isQueen()){elem->isPieceWhite() ? fen.append("Q") : fen.append("q");}
		if(elem->isKing()){elem->isPieceWhite() ? fen.append("K") : fen.append("k");}
		if(elem->isEmpty()){empty_count++;}

		// castling
		if(!board[4]->getPieceMoveInfo() && !board[0]->getPieceMoveInfo()){b_castle_q = true;}
		if(!board[7]->getPieceMoveInfo() && !board[0]->getPieceMoveInfo()){b_castle_k = true;}
		if(!board[56]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo()){w_castle_q = true;}
		if(!board[63]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo()){w_castle_k = true;}

		sprintf(temp, "%i/", empty_count);
		if(elem_count % 8 == 7 && elem_count != 63)
		{
			if(empty_count > 0)
			{
				fen.append(temp);
				empty_count=0;
			} 
			else
			{
				fen.append("/");
			}
		}
		elem_count++;
	}
	fen.append(chess.getTurn() == 2 ? " w " : " b ");

	if(w_castle_k){fen.append("K");}
	if(w_castle_q){fen.append("Q");}
	if(b_castle_k){fen.append("k");}
	if(b_castle_q){fen.append("q");}

	// move count
	fen.append(" - ");
	sprintf(temp, "%i ", white_turns);
	fen.append(temp);
	sprintf(temp, "%i", black_turns);
	fen.append(temp);

	cout << fen << endl;
	exit(1);
	return fen;
}

int main()
{
	char filename[1000];
	int file_num = 1, num_failed = 0, white_turns = 0, black_turns = 0;
	vector<string> failed_tests;
	vector<int> test_case_num;

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
			printf("\n\n===================== TEST CASE %i ==================== \n\n", file_num);
			SetConsoleTextAttribute(hConsole, DEFAULT);

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
		            chess.getTurn() == 2 ? white_turns++ : black_turns++;
					boardFenConverter(chess.getBoard(), white_turns, black_turns);
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
		for(itr = failed_tests.begin(); itr != failed_tests.end(); itr++)
		{
			cout << *itr;
			SetConsoleTextAttribute(hConsole, YELLOW);
			cout << " (Test Case " << test_case_num[itr - failed_tests.begin()] << ")" << endl;
			SetConsoleTextAttribute(hConsole, DEFAULT);
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

		