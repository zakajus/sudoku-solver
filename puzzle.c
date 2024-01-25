/**
 * @file puzzle.c
 * @author Kajus Zakaras (kajus.z@tuta.io)
 * @brief Sudoku puzzle logic, dynamic array management
 * @version 1.00
 * @date 2024-01-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "./dependencies.h"
#include "./puzzle.h"
#include "./ui.h"


/// @brief Generates bitmap in puzzle from its grid array values
/// @param puzzle Puzzle for which to generate the bitmap
void generateBitmap(Puzzle *puzzle) {
    int map[GRID_SIZE][GRID_SIZE];
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (puzzle->grid[i][j] > 0) {
                puzzle->map[i][j] = 1;
            }
        }
    }
}


/// @brief Replaces user grid with an unmodified copy
/// @param puzzle Puzzle for which to reset the user grid
void generateUserGrid(Puzzle *puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            puzzle->userGrid[i][j] = puzzle->grid[i][j];
        }
    }
}


/// @brief Change user grid square if bitmap allows
/// @param puzzle The puzzle to modify
/// @param x Cartesian x coordinate of square
/// @param y Cartesian y coordinate of square
/// @param value Value to write
/// @return -1: unchanged due to bitmap; 0: changed successfully
int changeValue(Puzzle *puzzle, int x, int y, int value) {
    if (puzzle->map[GRID_SIZE-y][x-1] == 0) {
        puzzle->userGrid[GRID_SIZE-y][x-1] = value;
        return 0;
    }
    return -1; 
}


/// @brief Dynamically appends puzzle to array and handles memory allocation
/// @param puzzle Puzzle to append
/// @param puzzleArray Array to append to
/// @param puzzleCount Array size, increments +1 after automatically
void addPuzzle(Puzzle puzzle, PuzzleArray *puzzleArray, int *puzzleCount) {
    *puzzleArray = realloc(*puzzleArray, (*puzzleCount + 1) * sizeof(Puzzle));
    if (*puzzleArray == NULL) {
        fprintf(stderr, "%s", translate("ERROR_MEMORY_ALLOCATION"));
        exit(1);
    }
    generateBitmap(&puzzle);
    generateUserGrid(&puzzle);
    (*puzzleArray)[*puzzleCount] = puzzle;
    *puzzleCount = *puzzleCount + 1;
}


/// @brief Checks if puzzle user grid row includes 1-9 exactly once
/// @param puzzle Puzzle to check
/// @param row Row to check
/// @return 1: Proper row; 0: Improper row
int checkRow(Puzzle puzzle, int row) {
    for (int num = 1; num <= GRID_SIZE; ++num) {
        int count = 0;
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (puzzle.userGrid[row][j] == num) {
                ++count;
            }
        }
        if (count != 1) {
            return 0;
        }
    }
    return 1;
}


/// @brief Checks if puzzle user grid column includes 1-9 exactly once
/// @param puzzle Puzzle to check
/// @param col Column to check
/// @return 1: Proper column; 0: Improper column
int checkColumn(Puzzle puzzle, int col) {
    for (int num = 1; num <= GRID_SIZE; ++num) {
        int count = 0;
        for (int i = 0; i < GRID_SIZE; ++i) {
            if (puzzle.userGrid[i][col] == num) {
                ++count;
            }
        }
        if (count != 1) {
            return 0;
        }
    }
    return 1;
}


/// @brief Checks if puzzle puzzle 3x3 subgrid includes 1-9 exactly once
/// @param puzzle Puzzle to check
/// @param startRow Subgrid start row
/// @param startCol Subgrid start column
/// @return 1: Proper subgrid; 0: Improper subgrid
int checkBox(Puzzle puzzle, int startRow, int startCol) {
    for (int num = 1; num <= GRID_SIZE; ++num) {
        int count = 0;
        for (int i = startRow; i < startRow + 3; ++i) {
            for (int j = startCol; j < startCol + 3; ++j) {
                if (puzzle.userGrid[i][j] == num) {
                    ++count;
                }
            }
        }
        if (count != 1) {
            return 0;
        }
    }
    return 1;
}


/// @brief Checks if puzzle is fully solved
/// @param puzzle Puzzle to check
/// @return 1: Solved; 0: Unsolved
int isSudokuSolved(Puzzle puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        if (!checkRow(puzzle, i) || !checkColumn(puzzle, i)) {
            return 0;
        }
    }
    for (int i = 0; i < GRID_SIZE; i += 3) {
        for (int j = 0; j < GRID_SIZE; j += 3) {
            if (!checkBox(puzzle, i, j)) {
                return 0;
            }
        }
    }
    return 1;
}


/// @brief Checks if num would appear once in row, column, subgrid
/// @param puzzle Puzzle to check
/// @param row Row to check
/// @param col Column to check
/// @param num Number that would be written
/// @return 1: Safe to write; 0: Unsafe to write
/// @note Functions checkColumn(), checkRow(), and checkBox() do not work here since they check every number
int isSquareSafe(Puzzle *puzzle, int row, int col, int num) {
    for (int x = 0; x < GRID_SIZE; x++)
        if (puzzle->userGrid[row][x] == num)
            return 0;

    for (int x = 0; x < GRID_SIZE; x++)
        if (puzzle->userGrid[x][col] == num)
            return 0;

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (puzzle->userGrid[i + startRow][j + startCol] == num)
                return 0;

    return 1;
}


/// @brief Solves user grid of puzzle
/// @param puzzle Puzzle to solve
/// @param row Call with 0, used by recursive calls
/// @param col Call with 0, used by recursive calls
/// @return 0: unsolvable if returned by initial call; used to trigger backtrack in recursion
/// @details Uses a backtracking (brute-force) algorithm. More optimized algorithms (e.g. stochastic search) are outside the scope of this project.
int solveSudokuUserGrid(Puzzle *puzzle, int row, int col) {
    if (row == GRID_SIZE - 1 && col == GRID_SIZE)
        return 1;

    if (col == GRID_SIZE) {
        row++;
        col = 0;
    }

    if (puzzle->userGrid[row][col] > 0)        
        return solveSudokuUserGrid(puzzle, row, col + 1);

    for (int num = 1; num <= GRID_SIZE; num++) {
        if (isSquareSafe(puzzle, row, col, num)) {
            puzzle->userGrid[row][col] = num;

            if (solveSudokuUserGrid(puzzle, row, col + 1))
                return 1;
        }

        puzzle->userGrid[row][col] = 0;  // undo the current square for backtracking
    }

    return 0;
}


/// @brief Calculates the number of solved puzzles in array
/// @param puzzleArrayCount  Array size
/// @param puzzleArray Array to check
/// @return Number of solved puzzles in array
/// @note Can be optimized in the future by storing .solved value in Puzzle structure
int countSolvedSudokus(int puzzleArrayCount, PuzzleArray puzzleArray) {
    int solvedCount = 0;
    for (int i = 0; i < puzzleArrayCount; ++i) {
        if (isSudokuSolved(puzzleArray[i]) == 1) {
            ++solvedCount;
        }
    }
    return solvedCount;
}


/// @brief Generates valid random values for subgrids across the primary diagonal
/// @param puzzle Puzzle to fill
/// @details Shuffled using Fisher-Yates algorithm, relies on <time.h> to seed on launch
void fillUserGridDiagonal(Puzzle* puzzle) {
    for (int i = 0; i < GRID_SIZE; i += SUBGRID_SIZE) {
        int numbers[GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; ++j) {
            numbers[j] = j + 1;
        }

        // Shuffle
        for (int j = GRID_SIZE - 1; j > 0; --j) {
            int r = rand() % (j + 1);
            int temp = numbers[j];
            numbers[j] = numbers[r];
            numbers[r] = temp;
        }

        for (int j = 0; j < SUBGRID_SIZE; ++j) {
            for (int k = 0; k < SUBGRID_SIZE; ++k) {
                puzzle->userGrid[i + j][i + k] = numbers[j * SUBGRID_SIZE + k];
            }
        }
    }
}


/// @brief Clears user grid squares until n clues remain
/// @param puzzle Puzzle to modify
/// @param n How many clues (non-empty squares) should remain 
void setNCluesInUserGrid(Puzzle* puzzle, int n) {
    srand(time(NULL));

    int clues = 0;
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (puzzle->userGrid[i][j] != 0) {
                clues++;
            }
        }
    }

    // Clear squares
    while (clues > n) {
        int i = rand() % GRID_SIZE;
        int j = rand() % GRID_SIZE;
        if (puzzle->userGrid[i][j] != 0) {
            puzzle->userGrid[i][j] = 0;
            clues--;
        }
    }
}


/// @brief Copies values from user grid to grid in puzzle
/// @param puzzle Puzzle to modify
void copyUserGridtoGrid(Puzzle *puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            puzzle->grid[i][j] = puzzle->userGrid[i][j];
        }
    }
}


/// @brief Generates and appends a valid puzzle with specified number of clues to array
/// @param puzzleArrayPtr Array to append to
/// @param puzzleCountPtr Array size
/// @param clues Number of clues puzzle will have (n>=17)
/// @note Make sure new puzzle is properly allocated in stack - current method works based on testing
/// @details Dependant on helper functions: 
/// \ generateUserGrid(), fillUserGridDiagonal(), solveSudokuUserGrid(), setNCluesInUserGrid(),
/// \ copyUserGridtoGrid(), generateBitmap(), addPuzzle()
void generatePuzzle(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, int clues) {
    // needed; otherwise puzzle not properly initalized
    // alternative is to pass empty puzzle from main --> more memory used in call stack
    Puzzle newPuzzle = {{
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
        }};

    generateUserGrid(&newPuzzle);
    fillUserGridDiagonal(&newPuzzle);
    solveSudokuUserGrid(&newPuzzle, 0, 0);
    setNCluesInUserGrid(&newPuzzle, clues);
    copyUserGridtoGrid(&newPuzzle);
    generateBitmap(&newPuzzle);
    addPuzzle(newPuzzle, puzzleArrayPtr, puzzleCountPtr);
}


/// @brief Deletes nth puzzle from array and reallocates memory
/// @param puzzleArrayPtr Array to delete from
/// @param puzzleCountPtr Array size, increments -1 after automatically
/// @param n nth puzzle to delete, 
void deleteNthPuzzle(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, int n) {
    for (int i = n; i < *puzzleCountPtr - 1; ++i) {
        (*puzzleArrayPtr)[i] = (*puzzleArrayPtr)[i + 1];
    }
    *puzzleArrayPtr = realloc(*puzzleArrayPtr, (*puzzleCountPtr - 1) * sizeof(Puzzle));
    if ((*puzzleCountPtr - 1) > 0 && *puzzleArrayPtr == NULL) {
        printf("%s\n", translate("ERROR_MEMORY_ALLOCATION"));
        exit(1);
    }
    *puzzleCountPtr = *puzzleCountPtr - 1;
}