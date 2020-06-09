#include <gtest/gtest.h>
#include <iostream>

#include "chess.cpp"

using namespace std;

class ChessTest : public ::testing::Test
{
public:
	Chess *obj;
	void SetUp()
	{
		obj = new Chess;
	}

	void TearDown()
	{
		delete obj;
	}
};

TEST_F(ChessTest, name)
{
	ASSERT_EQ(obj->getCheck(), false);
}

TEST_F(ChessTest, name2)
{
	ASSERT_EQ(obj->getCheck(), true);
}

// -lgtest_main does this for you automatically to avoid writing main
int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	cout << RUN_ALL_TESTS();
	return 0;
}