#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>

#define GRID_SIZE 9
#define LINE_MAX 60
#define BUFFER_SIZE 64

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[32m"

// displayStrings
char menuOptions_menuChange[][LINE_MAX] = {"q : Back\n", "x y value : change value at coordinate\n", "Enter selection: "};
char menuOptions_choosePuzzle[][LINE_MAX] = {"q : Back\n", "x : Play x puzzle\n", "Enter selection: "};

typedef struct Puzzle {
    int grid[GRID_SIZE][GRID_SIZE];
    int map[GRID_SIZE][GRID_SIZE];
} Puzzle;

void clearDisplay() {
    printf("\033[H\033[J");
}

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

void copyPuzzle(Puzzle puzzle, Puzzle *newPuzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            newPuzzle->grid[i][j] = puzzle.grid[i][j];
            newPuzzle->map[i][j] = puzzle.grid[i][j];
        }
    }
}

int changeValue(Puzzle *puzzle, int x, int y, int value) {
    if (puzzle->map[GRID_SIZE-y][x-1] == 0) {
        puzzle->grid[GRID_SIZE-y][x-1] = value;
        return 0;
    }
    return -1; 
}

void displayPuzzle(Puzzle puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        if ((i % 3 == 0) && (i > 0)) {
            printf("|-----------------------|\n");
        }
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (j % 3 == 0) {
                printf("| ");
            }
            if (puzzle.map[i][j] == 1){
                printf(ANSI_COLOR_MAGENTA "%d " ANSI_COLOR_RESET, puzzle.grid[i][j]); 
            }
            else {
                printf("%d ", puzzle.grid[i][j]); 
            }
              
        }
        printf("|\n");
    }
    printf("\n");
}

int isSudokuSolved(Puzzle puzzle) {
    /*
    In order for a puzzle to be solved, 3 conditions must be satisfied:
    1. Numbers 1-9 must appear exactly once in each row
    2. Numbers 1-9 must appear exactly once in each column
    3. Numbers 1-9 must appear exactly once in each 3x3 box 
    */
    bool solved = 0;
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
                    return 0;
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
                    return 0;
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
                        return 0;
                    }
                }
            }
        }

    return 1;
}

void displayMenu(char strArray[][LINE_MAX], int arrSize) {
    for (int i = 0; i < arrSize; ++i) {
        printf("%s", strArray[i]);
    } 
}

void menuPlay(Puzzle puzzle) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int x, y, val;
    generateBitmap(&puzzle);
    while (1) {
        
        if (isSudokuSolved(puzzle)) {
            printf(ANSI_COLOR_GREEN "This puzzle has been solved\n" ANSI_COLOR_RESET);
            printf("\n");
        }
        displayPuzzle(puzzle);
        displayMenu(menuOptions_menuChange, 3);
        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d %d %d", &x, &y, &val) == 3) {
            if (changeValue(&puzzle, x, y, val) == -1) {
                clearDisplay();
                printf("Cannot change hint values!\n");
            }
            else {
                clearDisplay();
                printf("Value at (%i, %i) changed to %i successfully!\n", x, y, val);
            }
        }
        else if (buffer[0] == 'q') {
            clearDisplay();
            break;
        }
        else {
            clearDisplay();
            printf("Invalid input!\n");
        }
    }
}

void menuChoosePuzzle(Puzzle **puzzleArray, int puzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int selection;
    char selectionChar;
    while (1) {
        
        printf("%d puzzles loaded\n", puzzleCount);
        displayMenu(menuOptions_choosePuzzle, 3);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection > 0 && selection <= puzzleCount) {
                menuPlay((*puzzleArray)[selection-1]);
            }
            else {
                clearDisplay();
                printf("Puzzle does not exist\n\n");
            }
        }
        else if (sscanf(buffer, "%c", &selectionChar) == 1) {
            if (selectionChar == 'q') {
                clearDisplay();
                break;
            }
            else {
                clearDisplay();
                printf("Invalid input\n\n");
            }
        }
        else {
            clearDisplay();
            printf("Invalid input\n\n");
        }
    }
    
    
}

void addPuzzle(Puzzle puzzle, Puzzle **puzzleArray, int *puzzleCount) {
    *puzzleArray = realloc(*puzzleArray, (*puzzleCount + 1) * sizeof(Puzzle));
    if (*puzzleArray == NULL) {
        printf("Memory allocation failure\n");
        exit(1);
    }
    (*puzzleArray)[*puzzleCount] = puzzle;
    *puzzleCount = *puzzleCount + 1;
}

void removePuzzle(Puzzle **puzzleArray, int *puzzleCount) {
    if (*puzzleCount > 0) {
        *puzzleArray = realloc(*puzzleArray, (*puzzleCount - 1) * sizeof(Puzzle));
        if ((*puzzleCount - 1) > 0 && *puzzleArray == NULL) {
            printf("Memory allocation failure\n");
            exit(1);
        }
        *puzzleCount = *puzzleCount - 1;
    }
    else {
        printf("Cannot remove puzzle from empty list\n");
    }
    
}

int main() {
    int puzzleCount = 0;
    Puzzle *puzzleArray = NULL;
    Puzzle exWrong917 = {{ // 1,1 --> 7
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {0, 4, 5, 2, 8, 6, 3, 1, 9}
        }};

    addPuzzle(exWrong917, &puzzleArray, &puzzleCount);
    addPuzzle(exWrong917, &puzzleArray, &puzzleCount);
    printf("%d\n", puzzleCount);
    // displayPuzzle((puzzleArray)[0]);
    // removePuzzle(&puzzleArray, &puzzleCount);
    printf("%d\n", puzzleCount);
    menuChoosePuzzle(&puzzleArray, puzzleCount);
    // generateBitmap(&exWrong917);
    // menuChange((puzzleArray)[0]);
    
    // char menuOptions1[][LINE_MAX] = {"1. Play Sudoku\n", "2. Stats\n", "3. Quit\n", "Enter selection (1-3): "};
    // displayMenu(menuOptions1, 4);

    free(puzzleArray);

    return 0;
}
