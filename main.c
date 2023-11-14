#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#define GRID_SIZE 9

typedef struct Puzzle {
    int grid[GRID_SIZE][GRID_SIZE];
} Puzzle;

/*
In order for a puzzle to be solved, 3 conditions must be satisfied:
1. Numbers 1-9 must appear exactly once in each row
2. Numbers 1-9 must appear exactly once in each column
3. Numbers 1-9 must appear exactly once in each 3x3 box 
*/
bool isSudokuSolved(Puzzle puzzle) {
    bool solved = false;
        // Checks lines
        for (int i = 0; i < GRID_SIZE; ++i) { // for row
            for (int num = 1; num <= GRID_SIZE; ++num) { // for numbers 1-9
                int count = 0;  
                for (int j = 0; j < GRID_SIZE; ++j) { // for element
                    if (puzzle.grid[i][j] == num) {
                        ++count;
                    }
                }
                if (count != 1) {
                    return false;
                }
            }
        }
        // Checks columns
        for (int j = 0; j < GRID_SIZE; ++j) { 
            for (int num = 1; num <= GRID_SIZE; ++num) {
                int count = 0;
                for (int i = 0; i < GRID_SIZE; ++i) {
                    if (puzzle.grid[i][j] == num) {
                        ++count;
                    } 
                }
                if (count != 1) {
                    return false;
                }
            }
        } 
        // Checks grids
        for (int vertPadding = 0; vertPadding <= GRID_SIZE-3; vertPadding += 3) {
            for (int horzPadding = 0; horzPadding <= GRID_SIZE-3; horzPadding += 3) {
                for (int num = 1; num <= GRID_SIZE; ++num) {
                    int count = 0;
                    for (int i = horzPadding; i < horzPadding+3; ++i) {
                        for (int j = vertPadding; j < vertPadding+3; ++j) {
                            if (puzzle.grid[i][j] == num) {
                                ++count;
                            }
                        }
                    }
                    if (count != 1) {
                        return false;
                    }
                }
            }
        }

    return true;
}

int main()
{
    Puzzle exSolved = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 2, 8, 6, 3, 1, 9}
        }};
    
    Puzzle exWrong1 = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 5, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 3, 8, 6, 3, 1, 9}
        }};

    Puzzle exWrong2 = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 9},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 1, 2, 8, 6, 3, 1, 9}
        }};

    Puzzle exHas0 = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
        }};

    assert(isSudokuSolved(exSolved) == true);
    assert(isSudokuSolved(exWrong1) == false);
    assert(isSudokuSolved(exWrong1) == false);
    assert(isSudokuSolved(exHas0) == false);

    return 0;
}
