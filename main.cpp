#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


using namespace std;

//directional deltas
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};


/* We want to have our board display various states instead of zeros and ones.
 Creating an enum allows us to build a custom class with custom outputs.
 The enum State will be used to describe the state of the baord instead of integers.
 All the functions will be refactored to accept a State object instead of an int*/
enum class State {kEmpty, kObstacle, kClosed, kPath, kStart, kFinish};

/* Function that takes an enum and returns the state as a string value
 * Enum is declared above. State {kEmpty, kObstacle}*/
string CellString(enum State cell) {
    switch(cell) {
        case State::kObstacle: return "⛰   ";
        case State::kStart: return "🚦   ";
        case State::kFinish: return "🏁   ";
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


/* Compare function accepts two vector<int> nodes as arguments. Function will return true
 * if the f-value of the first argument is greater than f-value of second argument, otherwise return false*/

bool Compare(vector<int> node1, vector<int> node2) {
    int f1 = node1[2] + node2[3]; // f1 = g1 + h1
    int f2 = node1[2] + node2[3]; // f2 = g2 + h2

    return f1 > f2;
}

/*CellSort function uses the Compare function to determine the sorting order*/

void CellSort(vector<vector<int>> *v) {
    sort(v->begin(), v->end(), Compare);
}
/* AddToOpen function accepts four ints {x, y, g, h}, reference to one 2D vector of ints for open nodes,
 reference to one 2D vector State for the grid. Function performs two things: creates a vector<int> node with {x, y, g, h}
 and push the node to the back of the open vector, sets the grid value for the x and y coordinates to the enum value kClosed.*/

void AddToOpen( int x,  int y,  int g,  int h, vector<vector<int>> &open_nodes, vector<vector<State>> &grid) {

    // Create a vector<int> node with {x, y, g, h}
    vector<int> node = {x, y, g, h};

    // Push back the new node to the 2D vector of open nodes
    open_nodes.push_back(node);

    // Set the grid value for the x and y coordinates to the enum value kClosed
    grid[x][y] = State::kClosed;
}

/* Heuristic function, takes four ints as arguments. The ints represent two pairs
  of 2D coordinates (x1, y1, x2, y2). Function returns an int which is the
  Manhattan Distance from one coordinate to the other: |x2-x1| + |y2-y1|
  */

int Heuristic( int x1,  int y1,  int x2,  int y2) {
    int manhattan_distance = abs(x2 - x1) + abs(y2 - y1);
    return manhattan_distance;
}

bool CheckValidCell(int x, int y, vector<vector<State>> grid) {
    // Check the (x,y) coordinate pair is on the grid
    bool on_grid_x = (x >= 0 && x < grid.size());
    bool on_grid_y = (y >= 0 && y < grid[0].size());

    // Check that grid at (x,y) is kEmpty (default case if grid cell is not kClosed or kObstacle)
    if (on_grid_x && on_grid_y)
        // If both conditions are true, return true.
        return grid[x][y] == State::kEmpty;
    return false;

}

void ExpandNeighbors(const vector<int> &current_node, int goal[2], vector<vector<int>> &open_nodes, vector<vector<State>> &grid) {
    // Get current node's data {x, y, g}
    int x = current_node[0];
    int y = current_node[1];
    int g = current_node[2];

    //Loop through current node's potential neighbors. Use the directional deltas to check values
    for (int i =0; i < 4; i++) {
        int x2 = x + delta[i][0];
        int y2 = y + delta[i][1];

        // Check that the potential neighbor's x2 and y2 values are on the grid and not closed.
        if (CheckValidCell(x2, y2, grid)) {
            // Increment g value and add neighbor to open list
            int g2 = g + 1;
            int h2 = Heuristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g2, h2, open_nodes, grid);
        }
    }

}

/* Function that takes a 2D State vector and two int arrays[2] as arguments.
 returns a 2D board vector*/

vector<vector<State>> Search(vector<vector<State>> grid, int initial_point[2], int goal_point[2]) {



    // Create the vector of open nodes
    vector<vector<int>> open {};

    // Initialize the starting node
    int x = initial_point[0];
    int y = initial_point[1];

    // Set the initial cost (g) to zero
    int g = 0;

    // Get the Heuristic value
    int h = Heuristic(initial_point[0], initial_point[1], goal_point[0], goal_point[1]);

    // Call AddToOpen to create the first node
    AddToOpen(x, y, g, h, open, grid);

    // While open vector is not empty
    while (not open.empty()) {

        /* Call CellSort to sort open list in desending order to have
        node with lowest heuristic value at the end*/
        CellSort(&open);

        // Get the last node from the open vector
        vector<int> current_node = open.back();

        // Remove the last node from the open vector
        open.pop_back();

        // Get the x & y values from current_node
        int cur_x = current_node[0];
        int cur_y = current_node[1];

        // Set grid[x][y] to kPath
        grid[cur_x][cur_y] = State::kPath;

        if (cur_x == goal_point[0] &&  cur_y == goal_point[1]) {
            // Set initial coordinates in grid to kStart
            grid[initial_point[0]][initial_point[1]] = State::kStart;

            // Set goal coordinates in grid to kFinish
            grid[goal_point[0]][goal_point[1]] = State::kFinish;
            return grid;
        }
        else {
            ExpandNeighbors(current_node, goal_point, open, grid);
        }
    }

    cout << "No path found!" << "\n";
    return vector<vector<State>> {};
}


    int main() {

    int init[2]{0,0};
    int goal[2]{4, 5};

    // Store the output of ReadBoardFile in the "board" variable
    vector<vector<State>> board = ReadBoardFile("/Users/jessekniss/CLionProjects/section1/1.board");
    auto solution = Search(board, init, goal);
    PrintBoard(solution);
}