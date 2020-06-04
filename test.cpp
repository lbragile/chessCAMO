#include "chess.h"

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
	if(board[63]->isRook() && board[60]->isKing() && board[60]->isSameColor(63) && !board[60]->getPieceMoveInfo() && !board[63]->getPieceMoveInfo()){fen.append("K");}
	if(board[56]->isRook() && board[60]->isKing() && board[60]->isSameColor(56) && !board[56]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo()){fen.append("Q");}
	if(board[7]->isRook() && board[4]->isKing() && board[4]->isSameColor(7) && !board[4]->getPieceMoveInfo() && !board[7]->getPieceMoveInfo()){fen.append("k");}
	if(board[0]->isRook() && board[4]->isKing() && board[4]->isSameColor(0) && !board[0]->getPieceMoveInfo() && !board[4]->getPieceMoveInfo()){fen.append("q");}

	return fen;
}

int main()
{
	int src, dest;
	string path = "test_cases/*.txt"; //../unit_cases/*.txt (for single tests) or test_cases/*.txt (for all test cases made)

	char filename[1000], text[100];
	int file_num = 0, num_failed = 0;

	vector<string> failed_tests, fen_obtained, fen_expected;
	vector<int> test_case_num;

	string fen_expected_input;

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, DEFAULT);
	
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(path.c_str(), &FindFileData);

	if(hFind == INVALID_HANDLE_VALUE){cout << "No files found" << endl;	exit(0);}
	else
	{
		do
		{
			sprintf(text, "\n\n===================== TEST CASE %i ====================\n\n", file_num+1);
			chess.printMessage(text, YELLOW);

			// make sure the board is empty prior to adding pieces AND all game ending conditions are reset
			Chess reset;
			chess = reset;

			// Create 8x8 default board
			chess.boardInit();

			// insert value to the end
			sprintf(filename,"test_cases/%s", FindFileData.cFileName);
			ifstream myfile(filename); //opening the file.
		    if(myfile.is_open()) //if the file is open
		    {
		    	getline(myfile, fen_expected_input);
		    	fen_expected.push_back(fen_expected_input);
		        while(!myfile.eof() && !chess.getCheckmate() && !chess.getStalemate()) //while the end of file is NOT reached or game is not finished
		        {	
		        	chess.printMessage("\nEnter a source AND destination square in [0, 63]: ", PINK);
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

	chess.printMessage("\n\n\nTest Case Summary\n", YELLOW);

	sprintf(text,"Passed: %i/%i", file_num-num_failed, file_num);
	chess.printMessage(text, GREEN);

	cout << " | ";

	sprintf(text,"Failed: %i/%i\n\n", num_failed, file_num);
	chess.printMessage(text, RED);

	if(!failed_tests.empty())
	{
		chess.printMessage("Failed Cases:\n", YELLOW);

		vector<string>::iterator itr;
		int test_case;
		for(itr = failed_tests.begin(); itr != failed_tests.end(); itr++)
		{
			test_case = test_case_num[itr - failed_tests.begin()];
			cout << *itr;

			sprintf(text, " (Test Case %i)\n", test_case+1);
			chess.printMessage(text, YELLOW);

			sprintf(text, "Expected FEN: %s\n", fen_expected[test_case].c_str());
			chess.printMessage(text, GREEN);

			sprintf(text, "Obtained FEN: %s\n", fen_obtained[test_case].c_str());
			chess.printMessage(text, RED);
		}
	}
	
	return 0;
}		