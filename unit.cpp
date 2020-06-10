#include <gtest/gtest.h>
#include <iostream>

#include "chess.h"

using namespace std;
using namespace chessCAMO;

class ChessTest : public ::testing::Test
{
protected:
	void SetUp()
	{
		std::cout.setstate(std::ios_base::failbit);
		Chess reset;
        chess = reset;
		chessCAMO::boardInit();
		chess.makeMove(55, 36);
		chess.makeMove(12, 28);
		chess.makeMove(51, 35);
		chess.makeMove(11, 27);
	}

	void TearDown()
	{
		std::cout.clear();
	}
};

// class PossibleMoveTest : public ::testing::Test
// {
// protected:
// 	void SetUp()
// 	{
// 		std::cout.setstate(std::ios_base::failbit);
// 		Chess reset;
//         chess = reset;
// 		chessCAMO::boardInit();
// 	}

// 	void TearDown()
// 	{
// 		std::cout.clear();
// 	}
// };
// TEST_F(PossibleMoveTest, isLegalMoveForAll)
// {	
// 	for(auto elem : chess.getBoard())
// 	{
// 		if(!elem->isEmpty())
// 		{
// 			if(elem->isPawn() && elem->isPieceBlack())
// 			{
// 				int sign = elem->isPieceWhite() ? 1 : -1;

// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()+(8*sign)), true);
// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()+(16*sign)), true);
// 			}

// 			if(elem->isRook())
// 			{
// 				int sign = elem->isPieceWhite() ? 1 : -1;

// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()-1), false);
// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()+1), false);
// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()+(16*sign)), false);
// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()+3), false); // castle
// 				EXPECT_EQ(elem->isPossibleMove(elem->getPieceSquare()-4), false); // castle
// 			}
// 		}
// 	}
// }

TEST_F(ChessTest, makeMoveIntPawnSingle)
{
	int src = 48, dest = 40;

	chess.makeMove(src, dest);
	vector<Piece*> after_board = chess.getBoard();

	EXPECT_EQ(after_board[src]->getPieceSquare(), src);
	EXPECT_EQ(after_board[dest]->getPieceSquare(), dest);
	ASSERT_EQ(after_board[src]->isPawn(), false);
	ASSERT_EQ(after_board[dest]->isPawn(), true);
}

TEST_F(ChessTest, makeMoveIntPawnDouble)
{
	int src = 48, dest = 32;

	chess.makeMove(src, dest);
	vector<Piece*> after_board = chess.getBoard();

	EXPECT_EQ(after_board[src]->getPieceSquare(), src);
	EXPECT_EQ(after_board[dest]->getPieceSquare(), dest);
	ASSERT_EQ(after_board[src]->isPawn(), false);
	ASSERT_EQ(after_board[dest]->isPawn(), true);
}

TEST_F(ChessTest, PawnAttackEmptySquare)
{
	int src = 36, dest = 29;

	chess.makeMove(src, dest);
	vector<Piece*> after_board = chess.getBoard();

	EXPECT_EQ(after_board[src]->getPieceSquare(), src);
	EXPECT_EQ(after_board[dest]->getPieceSquare(), dest);
	ASSERT_EQ(after_board[src]->isPawn(), false);
	ASSERT_EQ(after_board[dest]->isPawn(), true);
}

// TEST_F(ChessTest, makeMoveIntRook)
// {
// 	int src = 63, dest = 47;

// 	vector<Piece*> after_board = chess.getBoard();

// 	EXPECT_EQ(after_board[src]->getPieceSquare(), src);
// 	EXPECT_EQ(after_board[dest]->getPieceSquare(), dest);
// 	ASSERT_EQ(after_board[dest]->isRook(), false);
// }

TEST_F(ChessTest, makeMoveIntKnightValid)
{
	int src = 62, dest = 47;

	chess.makeMove(src, dest);
	vector<Piece*> after_board = chess.getBoard();

	EXPECT_EQ(after_board[src]->getPieceSquare(), src);
	EXPECT_EQ(after_board[dest]->getPieceSquare(), dest);
	ASSERT_EQ(after_board[dest]->isKnight(), true);
}


TEST_F(ChessTest, isStalemate)
{
	EXPECT_EQ(chess.isStalemate(), false);
	chess.makeMove(52, 36);
	ASSERT_EQ(chess.isStalemate(), false);
}

TEST_F(ChessTest, isSameColor)
{
	int src = 52, dest = 36;

	vector<Piece*> current_board = chess.getBoard(), after_board;

	EXPECT_EQ(current_board[src]->isSameColor(dest), false);
	EXPECT_EQ(current_board[src]->isPieceWhite(), true);

	chess.makeMove(src, dest);
	after_board = chess.getBoard();

	ASSERT_EQ(after_board[src]->isPieceWhite(), false);
}

// -lgtest_main does this for you automatically to avoid writing main
// int main(int argc, char **argv)
// {
// 	::testing::InitGoogleTest(&argc, argv);
// 	return RUN_ALL_TESTS();
// }