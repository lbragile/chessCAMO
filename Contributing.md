# Contributing

Thank you so much for wanting to contribute to *chessCAMO*. Any constructive criticism regarding this project is welcomed and appreciated, as ultimately I love learning new things and getting better! It would be amazing if this project inspires or assists other users with their project ideas.

When contributing to this repository, please first discuss the change you wish to make via issue, email, or any other method with the owner(s) of this repository before making a change. 

Note that there is a code of conduct, please follow it in all your interactions with the project.

## What to Contribute

As this project is open-source, you can pick an choose what it is that you would like to contribute. This includes anything from improving the code (new function implementation, edits, comments, etc.), to adding test cases and/or any other files that you deem necessary for the successful completion of this project.

### How to Make Test Cases

Take a look at the implementation file "*test.cpp*". This implementation file is meant to be used as a test case checker to improve efficiency in debugging and will indicate how to produce an appropriate test case.

The main process is as follows:

1. One can set-up a position in common chess sites like lichess (https://lichess.org/editor) and extract the FEN at the bottom of the board.

2. Make a text file and pastes this FEN at the top, making sure to remove the last 6 characters. 

   **E.g.** *rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq **- 0 1***, becomes *rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq*.

3. On the next line, enter the corresponding source ('src') and destination ('dest') coordinates for a given move. 

   **E.g.** 52 36 (note the space) means moving from square 52 to 36.

4. Rinse and repeat step 3) until the position provided in step 1) is met. Make sure to not end on a blank line to avoid unnecessary warnings from the algorithm.

5. Name the file a meaningful name according to what is being tested. 

   **E.g.** to check if a given piece (Queen) delivers a checkmate => "*queenCheckmate.txt*"

6. Place test case files in a folder named "*./test_cases/*" (used by the algorithm).

#### Note

- ​	There is no draw or resign functionality here since this is meant for quick testing.
- ​	If an invalid move is made, the board is printed to allow debugging of the board state. 

## Pull Request Process

1. Ensure any install or build dependencies are removed before the end of the layer when doing a build.
2. Update the README.md with details of changes to the interface, this includes new environment variables and useful file locations.
3. Increase the version numbers in any examples files and the README.md to the new version that this Pull Request would represent. 
4. You may merge the Pull Request in once you have the sign-off of two other developers, or if you do not have permission to do that, you may request the second reviewer to merge it for you.

## Code of Conduct

### Pledge

In the interest of fostering an open and welcoming environment, I as contributor and maintainer pledge to making participation in this project and
community a harassment-free experience for everyone, regardless of age, body size, disability, ethnicity, gender identity and expression, level of experience, nationality, personal appearance, race, religion, or sexual identity and orientation.

### Standards

Examples of behavior that contributes to creating a positive environment include:

* Using welcoming and inclusive language
* Being respectful of differing viewpoints and experiences
* Gracefully accepting constructive criticism
* Focusing on what is best for the community
* Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

* The use of sexualized language or imagery and unwelcome sexual attention or advances
* Trolling, insulting/derogatory comments, and personal or political attacks
* Public or private harassment
* Publishing others' private information, such as a physical or electronic address, without explicit permission
* Other conduct which could reasonably be considered inappropriate in a professional setting

### Responsibilities

I am responsible for clarifying the standards of acceptable behavior and expected to take appropriate and fair corrective action in response to any instances of unacceptable behavior. I have the right and responsibility to remove, edit, or reject comments, commits, code, wiki edits, issues, and other contributions that are not aligned to this Code of Conduct, or to ban temporarily or permanently any contributor for other behaviors that they deem inappropriate, threatening, offensive, or harmful.

### Scope

This Code of Conduct applies both within project spaces and in public spaces when an individual is representing the project or its community. Examples of representing a project or community include using an official project e-mail address, posting via an official social media account, or acting as an appointed representative at an online or offline event. Representation of a project may be further defined and clarified by me.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be reported by contacting me at lbragile@sfu.ca. All complaints will be reviewed and investigated and will result in a response that is deemed necessary and appropriate to the circumstances. I am obligated to maintain confidentiality with regard to the reporter of an incident. Further details of specific enforcement policies may be posted separately.