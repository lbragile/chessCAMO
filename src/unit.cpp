/**
 * \page unit Unit Implementation File Description (GoogleTest FrameWork)
 * 
 * <b>Title</b><br>
 * <span>&emsp;&emsp;&emsp;unit.cpp</span>
 * \author Lior Bragilevsky<br>
 * 
 * <b>Related</b><br>
 * <span>&emsp;&emsp;&emsp;chess.h</span><br>
 * 
 * <b>Project</b><br>
 * <span>&emsp;&emsp;&emsp;chessCAMO</span><br>
 * \version \version_num
 * \date \today
 * 
 * This implementation file is meant to be used as a test case checker to improve efficiency in debugging.
 * The algorithm reads in text files that outline a given board position and feeds in the moves presented
 * in these files one-by-one until the end of each file. At the end, the algorithm compares the expected
 * FEN string value to the one obtained by making the corresponding moves. If these FEN strings are
 * identical then the algorithm increments the number of test cases passed, otherwise it increments the
 * number of test cases failed. These results are summarized at the end, when all the files are read and
 * processed, and if a "Failed Test Case" Summary is provided for each test case that failed.
 * 
 * Simply run <b>mingw32-make all_unit && unit --gtest_color=yes</b> on a Windows machine to start the test checking script.
 * 
 * <b> Include Addition Tests </b>
 *     1. A user can set-up a position in common chess sites like <a href="https://lichess.org/editor" target="__blank">lichess</a>
 *        and extract the FEN at the bottom of the board;
 *     2. The user then makes a text file and pastes this FEN at the top, making sure to remove the last
 *        6 characters. E.g. <i> rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq</i> <b>- 0 1</b>, becomes
 *        <i>rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq</i>;
 *     3. On the next line, the user enters the corresponding source ('src') and destination ('dest')
 *        coordinates for a given move. E.g. 52 36 (note the space) means moving from square 52 to 36;
 *     4. Rinse and repeat step 3 until the position provided in step 1 is met.
 *        Make sure to not end on a blank line to avoid unnecessary warnings from the algorithm;
 *     5. Name the file a meaningful name according to what is being tested.
 *        E.g. to check if a given piece (Queen) delivers a checkmate <span>&rarr;</span> <i>queenCheckmate.txt</i>;
 *     6. Place test case files in a folder named <b>./tests/</b> (used by the algorithm).

 * \note
 *     - There is no draw or resign functionality here since this is meant for quick testing.
 *     - If an invalid move is made, the board is printed to allow debugging of the board state.
 *     - Test cases 12 to 14 require user input to decide the promotion. To avoid errors please enter
 *       'q' (or 'Q'), 'n' (or 'N'), and 'b' (or 'B'), respectively. The algorithm will promote to
 *       appropriate piece color based on player's current move, regardless of the input's case.
*/

#include <iostream>
#include <gtest/gtest.h>

#include "chess.h"

// included in 'chess.h' but good to re-state
using namespace std;
using namespace chessCAMO;

// forward declaration
class ChessTest;

/*************************************************************************************/
/*                              FUNCTION DECLARATION                                 */
/*************************************************************************************/
/**
 * @brief      Converts a given board position into a <a href="https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation" target="__blank">FEN</a> string representation
 *
 * @param      chess  The chess object
 *
 * \pre
 * Start with an initialized board vector
 * 
 * \post
 * None
 * 
 * @return     The FEN string based on piece type and common FEN making rules
 */
string boardFenConverter(Chess * chess);

/**
 * @brief      Decides what type of character to append to the formed FEN string
 *
 * @param      fen          The FEN string that characters will be appended to (can be empty)
 * @param      empty_count  The number of empty squares found in the current row
 * @param[in]  next_char    The next character to append to 'fen', must be one of P, N, B, Q, K (upper case)
 * @param[in]  isWhite      True if the passed piece is white, False otherwise. Used to determine piece's letter case in 'fen'.
 * 
 * \pre
 * An empty 'fen' string
 * 
 * \post
 * Appends 'next_char' to 'fen' and returns updated variables by reference
 */
void appendFEN(string & fen, int & empty_count, char next_char, bool isWhite);

/*************************************************************************************/
/*                              TEST FIXTURE SETUP                                   */
/*************************************************************************************/
/**
 * @brief      This class describes a chess test for string or integer source and destination squares.
 *             It is used as a fixture to quickly set up tests without duplicating the code,
 *             by utilizing the overriden SetUp and TearDown functions.
 */ 
class ChessTest : public ::testing::Test
{
protected:
    /// create the testing object
    Chess *chess = new Chess;

    /**
     * \brief The source square of the piece to-be-moved
     * 
     * \note 
     * Coordinates are in [A1, H8] -> A1 is bottom left, H8 is top right
     */
    string src;

     /**
     * \brief The destination square of the piece to-be-moved
      * 
     * \note 
     * Coordinates are in [A1, H8] -> A1 is bottom left, H8 is top right
     */
    string dest;

    /// Expected FEN string for a given board representation before moves are made for it
    string fen_expected;

    /// Obtained FEN string for a given board representation after moves are made for it
    string fen_obtained;

    /**
     * @brief      Before a given test is executed, this sets up everything and initializes the variables
     *             Additionally, this performs the actual gameplay moves needed to obtain the final board representation
     *
     * @param      myfile  The file to read moves from
     */
    void SetUp(ifstream & myfile)
    {
        cout.setstate(std::ios_base::failbit); // surpress output
        chess->boardInit();

        /* -------------------- Act -------------------- */
        if(myfile.is_open()) //if the file is open
        {
           // read in the expected FEN (this is at the top of each test case file)
            getline(myfile, fen_expected);

            // read in the moves of the given test case file one line at a time
            // while the end of file is NOT reached and game is NOT finished (checkmate, stalemate, draw, resign)
            while(!myfile.eof() && !chess->getCheckmate() && !chess->getStalemate())
            {   
                chessCAMO::printMessage("\nEnter a source AND destination square in [A1, H8]: ", PINK); // for debugging purposes
                myfile >> src >> dest;

                cout << src << " " << dest << endl; // for debugging purposes
                chess->makeMove(chessCAMO::preProcessInput(src), chessCAMO::preProcessInput(dest), myfile);

                // prevent asking again after game is over
                if(!chess->getCheckmate() && !chess->getStalemate())
                {
                    chessCAMO::drawOrResign(chess, myfile);

                    // drawOrResign can set the checkmate flag to true if player chooses to resign or draw
                    // so if this happens break out of the while loop
                    if(chess->getCheckmate())
                        break;
                }
            }
            myfile.close(); //closing the file
        }
        else // test case file failed to open or doesn't exist
        {
            fen_expected = "-";
        }
    }

    /**
     * @brief      After a given test is completed, free the memory made by dynamic allocation.
     *             This ensures that the following test can dynamically allocate memory without
     *             causing a memory leak. 
     */
    void TearDown() override
    {
        cout.clear(); // enable output again
        delete chess;
    }
};


/*************************************************************************************/
/*                                       TESTS                                       */
/*************************************************************************************/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
TEST_F(ChessTest, unableToOpenFileString)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/unableToOpenFileString.txt");
    SetUp(myfile);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, "-");
}

TEST_F(ChessTest, stringInputsResignForCoverage)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/00-stringInputsResignForCoverage.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, stringInputsNDrawFeaturesForCoverage)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/00-stringInputsNDrawFeaturesForCoverage.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleAfterKingMoved)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/01-castleAfterKingMoved.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleAfterKingSideRookMoved)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/02-castleAfterKingSideRookMoved.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleAfterQueenSideRookMoved)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/03-castleAfterQueenSideRookMoved.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleKingSide)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/04-castleKingSide.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleQueenSide)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/05-castleQueenSide.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, kingMoveIntoCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/06-kingMoveIntoCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, queenMate)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/07-queenMate.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, queenCheckKingMoves)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/08-queenCheckKingMoves.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, queenCheckPieceDefends)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/09-queenCheckPieceDefends.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, queenCheckPieceDefends2)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/10-queenCheckPieceDefends2.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, queenCheckPieceDefends3)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/11-queenCheckPieceDefends3.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionQueenWhiteLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/12-pawnPromotionQueenWhiteLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionQueenWhiteUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/12-pawnPromotionQueenWhiteUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionKnightWhiteLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/13-pawnPromotionKnightWhiteLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionKnightWhiteUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/13-pawnPromotionKnightWhiteUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionBishopWhiteLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/14-pawnPromotionBishopWhiteLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionBishopWhiteUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/14-pawnPromotionBishopWhiteUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, trickyCastle)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/15-trickyCastle.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleWhenInCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/16-castleWhenInCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleThroughCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/17-castleThroughCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, castleIntoCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/18-castleIntoCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enPassantTakeRight)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/19-enPassantTakeRight.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enPassantTakeLeft)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/19-enPassantTakeLeft.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enPassantWithoutCapture)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/20-enPassantWithoutCapture.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, knightCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/21-knightCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, knightCheckKingMoves)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/22-knightCheckKingMoves.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, knightMate)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/23-knightMate.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedPieceBishop)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/24-pinnedPieceBishop.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedPieceQueen)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/25-pinnedPieceQueen.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedPiecePawnCanCapture)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/26-pinnedPiecePawnCanCapture.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, moveIntoKnightCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/27-moveIntoKnightCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, moveIntoPawnCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/28-moveIntoPawnCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, stalemate)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/29-stalemate.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckWithKnight)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/30-doubleCheckWithKnight.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckwithBishop)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/31-doubleCheckwithBishop.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckmateKnight)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/32-doubleCheckmateKnight.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckmateBishop)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/33-doubleCheckmateBishop.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckPawnKingMovesOutofCheck)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/34-doubleCheckPawnKingMovesOutofCheck.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckPawnKingChoosesToTake)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/35-doubleCheckPawnKingChoosesToTake.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, doubleCheckPawnKingMustTake)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/36-doubleCheckPawnKingMustTake.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedWithBishop)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/37-pinnedWithBishop.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedWithQueen)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/37-pinnedWithQueen.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pinnedWithRook)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/37-pinnedWithRook.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionRookWhiteLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/38-pawnPromotionRookWhiteLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionRookWhiteUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/38-pawnPromotionRookWhiteUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enPassantPawnRight)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/39-enPassantPawnLeft.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enPassantPawnLeft)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/40-enPassantPawnRight.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionQueenBlackLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/41-pawnPromotionQueenBlackLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionQueenBlackUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/41-pawnPromotionQueenBlackUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionRookBlackLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/42-pawnPromotionRookBlackLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionRookBlackUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/42-pawnPromotionRookBlackUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}


TEST_F(ChessTest, pawnPromotionBishopBlackLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/43-pawnPromotionBishopBlackLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionBishopBlackUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/43-pawnPromotionBishopBlackUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionKnightBlackLow)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/44-pawnPromotionKnightBlackLow.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, pawnPromotionKnightBlackUp)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/44-pawnPromotionKnightBlackUp.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, checkmateQueenBlack)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/45-checkmateQueenBlack.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, stalemateBlack)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/46-stalemateBlack.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, resignBlack)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/47-resignBlack.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, scholarMate)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/48-scholarMate.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

TEST_F(ChessTest, enpassantDeniedPawnMoves)
{
    /* ------------------ Arrange ------------------ */
    ifstream myfile("tests/49-enpassantDeniedPawnMoves.txt");
    SetUp(myfile);

    /* -------------------- Act -------------------- */
    // convert the final board position from a given test case file into a FEN string
    fen_obtained = boardFenConverter(chess);

    /* ------------------- Assert ------------------ */
    EXPECT_EQ(fen_expected, fen_obtained);
}

// TEST_F(ChessTest, scholarAttemptWithJustQueen)
// {
//     /* ------------------ Arrange ------------------ */
//     ifstream myfile("tests/50-scholarAttemptWithJustQueen.txt");
//     SetUp(myfile);

//      -------------------- Act -------------------- 
//     // convert the final board position from a given test case file into a FEN string
//     fen_obtained = boardFenConverter(chess);

//     /* ------------------- Assert ------------------ */
//     EXPECT_EQ(fen_expected, fen_obtained);
// }

// -lgtest_main does this for you automatically to avoid writing main
// int main(int argc, char **argv)
// {
//  ::testing::InitGoogleTest(&argc, argv);
//  return RUN_ALL_TESTS();
// }
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

/*************************************************************************************/
/*                              FUNCTION DEFINITION                                  */
/*************************************************************************************/
/**
 * @brief      Converts a given board position into a <a href="https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation" target="__blank">FEN</a> string representation
 *
 * @param      chess  The chess object
 *
 * \pre
 * Start with an initialized board vector
 * 
 * \post
 * None
 * 
 * @return     The FEN string based on piece type and common FEN making rules
 */   
string boardFenConverter(Chess * chess)
{
    int elem_count = 0, empty_count = 0;
    string fen;
    char temp[15];

    vector<Piece*> board = chess->getBoard();
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
    fen.append(chess->getTurn() == WHITE ? " w " : " b ");

    // castle handling (for both sides)
    /***** White player *****/
    if(board[63]->isRook() && board[60]->isKing() && board[60]->isSameColor(63, chess) && !board[60]->getPieceMoveInfo() && !board[63]->getPieceMoveInfo())
        fen.append("K");
    if(board[56]->isRook() && board[60]->isKing() && board[60]->isSameColor(56, chess) && !board[56]->getPieceMoveInfo() && !board[60]->getPieceMoveInfo())
        fen.append("Q");

    /***** Black player *****/
    if(board[7]->isRook() && board[4]->isKing() && board[4]->isSameColor(7, chess) && !board[4]->getPieceMoveInfo() && !board[7]->getPieceMoveInfo())
        fen.append("k");
    if(board[0]->isRook() && board[4]->isKing() && board[4]->isSameColor(0, chess) && !board[0]->getPieceMoveInfo() && !board[4]->getPieceMoveInfo())
        fen.append("q");

    return fen;
}

/**
 * @brief      Decides what type of character to append to the formed FEN string
 *
 * @param      fen          The FEN string that characters will be appended to (can be empty)
 * @param      empty_count  The number of empty squares found in the current row
 * @param[in]  next_char    The next character to append to 'fen', must be one of P, N, B, Q, K (upper case)
 * @param[in]  isWhite      True if the passed piece is white, False otherwise. Used to determine piece's letter case in 'fen'.
 * 
 * \pre
 * An empty 'fen' string
 * 
 * \post
 * Appends 'next_char' to 'fen' and returns updated variables by reference
 */
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