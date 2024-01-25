#include <assert.h>

#include "./puzzle.h"
#include "./dependencies.h"

int main() {

    Puzzle solved = {
        {}, {
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 2, 8, 6, 3, 1, 9}
        }
    };

    Puzzle unsolved = {
        {}, {
        {3, 1, 0, 5, 7, 0, 4, 9, 0},
        {0, 2, 9, 1, 0, 4, 7, 6, 0},
        {4, 0, 7, 6, 2, 9, 5, 0, 1},
        {2, 6, 3, 0, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 0, 3, 1, 2, 0},
        {8, 0, 1, 7, 9, 0, 6, 4, 3}, 
        {1, 3, 0, 9, 4, 0, 0, 5, 6},
        {6, 0, 2, 3, 5, 1, 8, 0, 4},
        {7, 4, 0, 0, 8, 0, 3, 1, 0}
        }
    };

    for (int i = 0; i < 9; ++i) {
        assert(checkColumn(solved, i) == 1);
        assert(checkColumn(unsolved, i) == 0);

        assert(checkRow(solved, i) == 1);
        assert(checkRow(unsolved, i) == 0);
    }
    
    for (int i = 0; i <= 6; i = i + 3) {
        for (int j = 0; j <= 6; j = j + 3) {
            assert(checkBox(solved, i, j) == 1);
            assert(checkBox(unsolved, i, j) == 0);
        }
    }

    solveSudokuUserGrid(&unsolved, 0 , 0);

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            assert(solved.userGrid[i][j] == unsolved.userGrid[i][j]);
        }
    }

    return 0;
}