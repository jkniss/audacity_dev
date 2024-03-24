#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

/* We want to have our board display various states instead of zeros and ones.
 Creating an enum allows us to build a custom class with custom outputs.
 The enum State will be used to describe the state of the baord instead of integers.
 All the functions will be refactored to accept a State object instead of an int*/
enum class State {kEmpty, kObstacle};

/* Function that takes an enum and returns the state as a string value
 * Enum is declared above. State {kEmpty, kObstacle}*/
string CellString(enum State cell) {
    switch(cell) {
        case State::kObstacle: return "⛰   ";
        default: return "0   ";
    }
}

/* Function that accepts a string as an argument and
 returns a vector of States after parsing the string */

vector<State> ParseLine(const string& a) {
    // read in the string
    istringstream my_stream(a);

    // variables for the char and int
    char c;
    int n;
    vector<State> row;

    // add the values to the vector
    while (my_stream >> n >> c && c == ',') {
        if (n == 0) {
            row.push_back(State::kEmpty);
        }
        else {
            row.push_back(State::kObstacle);
        }
    }
    // return the vector
    return row;
}

/* Function that accepts a filepath string to read in.
  Takes that file and uses the ParseLine function to
  parse the file data into a 2D vector of States and returns the 2D Vector*/

vector<vector<State>> ReadBoardFile(const string& path) {
    ifstream myfile (path);

    // Declare empty board variable
    vector<vector<State>> board{};
    if (myfile) {
        string line;
        while (getline(myfile, line)) {
            board.push_back(ParseLine(line));
        }
    }
    return board;
}


/* Function that accepts a 2D vector of states and prints out the result*/
void PrintBoard(const vector<vector<State>>& board) {
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {

            cout << CellString(board[i][j]);
        }
        cout << "\n";
    }
}


int main() {

    // Store the output of ReadBoardFile in the "board" variable
    vector<vector<State>> board = ReadBoardFile("/Users/jessekniss/CLionProjects/section1/1.board");
    PrintBoard(board);
}