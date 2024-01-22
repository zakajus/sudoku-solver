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
#define ANSI_COLOR_RED     "\x1b[31m"

// displayStrings
char menuOptions_menuPlay[][LINE_MAX] = {"q : Back\n", "x y value : change value at coordinate\n", "r : Reset puzzle\n\n", "Enter selection: "};
char menuOptions_choosePuzzle[][LINE_MAX] = {"q : Back\n", "# : Play # puzzle\n\n", "Enter selection: "};
char menuOptions_mainMenu[][LINE_MAX] = {"q : Save and exit\n", "1 : Play Sudoku\n", "2 : Sudoku solver\n", "3 : Stats\n", "r : Reset save data\n\n" "Enter selection: "};

typedef struct Puzzle {
    int grid[GRID_SIZE][GRID_SIZE];
    int userGrid[GRID_SIZE][GRID_SIZE]; // Modified by user, can be reset
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

void generateUserGrid(Puzzle *puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            puzzle->userGrid[i][j] = puzzle->grid[i][j];
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
        puzzle->userGrid[GRID_SIZE-y][x-1] = value;
        return 0;
    }
    return -1; 
}

void displayBanner() {
printf("\
  ___         _     _        \n \
/ __|_  _ __| |___| |___  _ \n \
\\__ \\ || / _` / _ \\ / / || |\n \
|___/\\_,_\\__,_\\___/_\\_\\_,_|\n \
-----------------------------\n\
");
}

void displayPuzzleGrid(Puzzle puzzle) {
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

void displayPuzzleUserGrid(Puzzle puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        if ((i % 3 == 0) && (i > 0)) {
            printf("|-----------------------|\n");
        }
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (j % 3 == 0) {
                printf("| ");
            }
            if (puzzle.map[i][j] == 1){
                printf(ANSI_COLOR_MAGENTA "%d " ANSI_COLOR_RESET, puzzle.userGrid[i][j]); 
            }
            else {
                printf("%d ", puzzle.userGrid[i][j]); 
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
                    if (puzzle.userGrid[i][j] == num) {
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
                    if (puzzle.userGrid[i][j] == num) {
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
                            if (puzzle.userGrid[i][j] == num) {
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

void addPuzzle(Puzzle puzzle, Puzzle **puzzleArray, int *puzzleCount) {
    *puzzleArray = realloc(*puzzleArray, (*puzzleCount + 1) * sizeof(Puzzle));
    if (*puzzleArray == NULL) {
        printf("Memory allocation failure\n");
        exit(1);
    }
    generateBitmap(&puzzle);
    generateUserGrid(&puzzle);
    (*puzzleArray)[*puzzleCount] = puzzle;
    *puzzleCount = *puzzleCount + 1;
}

void removeLastPuzzle(Puzzle **puzzleArray, int *puzzleCount) {
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

void removeNthPuzzle(Puzzle **puzzleArray, int *puzzleCount, int n) {
    if (*puzzleCount > 0 && n >= 0 && n < *puzzleCount) {
        for (int i = n; i < *puzzleCount - 1; i++) {
            (*puzzleArray)[i] = (*puzzleArray)[i + 1];
        }
        *puzzleArray = realloc(*puzzleArray, (*puzzleCount - 1) * sizeof(Puzzle));
        if ((*puzzleCount - 1) > 0 && *puzzleArray == NULL) {
            printf("Memory allocation failure\n");
            exit(1);
        }
        *puzzleCount = *puzzleCount - 1;
    }
    else {
        printf("Cannot remove puzzle: invalid index\n");
    }
}

void saveDataToFile(Puzzle *puzzleArray, int puzzleCount) {
    FILE *file = fopen("save.bin", "wb");
    if (file == NULL) {
        printf("Failed to open save.bin\n");
        exit(1);
    }
    if (fwrite(&puzzleCount, sizeof(puzzleCount), 1, file) != 1) {
        printf("Failed to write puzzleCount to save.bin\n");
        exit(1);
    }
    size_t itemsWritten = fwrite(puzzleArray, sizeof(Puzzle), puzzleCount, file);
    if (itemsWritten != puzzleCount) {
        printf("Failed to write puzzles to save.bin\n");
        exit(1);
    }
    fclose(file);
}

void loadDataFromFile(Puzzle **puzzleArray, int *puzzleCount) {
    FILE *file = fopen("save.bin", "rb");
    if (file == NULL) {
        printf("Failed to open save.bin\n");
        exit(1);
    }

    if (fread(puzzleCount, sizeof(*puzzleCount), 1, file) != 1) {
        printf("Failed to read puzzleCount from save.bin\n");
        exit(1);
    }

    *puzzleArray = malloc(sizeof(Puzzle) * (*puzzleCount));
    if (*puzzleArray == NULL) {
        printf("Failed to allocate memory for puzzleArray\n");
        exit(1);
    }

    size_t itemsRead = fread(*puzzleArray, sizeof(Puzzle), *puzzleCount, file);
    if (itemsRead != *puzzleCount) {
        printf("Failed to read puzzles from save.bin\n");
        exit(1);
    }

    fclose(file);
}

void initDataIfNoBinary(Puzzle *puzzleArray, int puzzleCount) {
    for (int i = 0; i < puzzleCount; ++i) {
        generateUserGrid(&(puzzleArray[i]));
        generateBitmap(&(puzzleArray[i]));
    }
    FILE *file = fopen("save.bin", "rb");
    if (file == NULL) {
        saveDataToFile(puzzleArray, puzzleCount);
    } 
    else {
        fclose(file);
    }
}

void displayMenu(char strArray[][LINE_MAX], int arrSize) {
    for (int i = 0; i < arrSize; ++i) {
        printf("%s", strArray[i]);
    } 
}

void menuPlay(Puzzle *puzzle) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int x, y, val;
    int firstLoop = 1;
    while (1) {
        
        if (isSudokuSolved(*puzzle)) {
            printf(ANSI_COLOR_GREEN "This puzzle has been solved\n" ANSI_COLOR_RESET);
            if (firstLoop == 1) {
                printf("\n");
            }
        }
        if (firstLoop == 0) {
            printf("\n");
        }
        firstLoop = 0;
        displayPuzzleUserGrid(*puzzle);
        displayMenu(menuOptions_menuPlay, 4);
        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d %d %d", &x, &y, &val) == 3) {
            if (val > GRID_SIZE) {
                clearDisplay();
                printf("Values cannot be higher than %d!\n", GRID_SIZE);
            }
            else if (changeValue(puzzle, x, y, val) == -1) {
                clearDisplay();
                printf("Cannot change hint values!\n");
            }
            else {
                clearDisplay();
                printf("Value at (%i, %i) changed to %i successfully!\n", x, y, val);
            }
        }
        else if (buffer[0] == 'r') {
            generateUserGrid(puzzle);
            clearDisplay();
            printf(ANSI_COLOR_RED "Puzzle has been reset!\n" ANSI_COLOR_RESET);
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
        displayBanner();
        printf("%d puzzles loaded\n\n", puzzleCount);
        displayMenu(menuOptions_choosePuzzle, 3);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection > 0 && selection <= puzzleCount) {
                menuPlay(&(*puzzleArray)[selection-1]);
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

void menuMain(Puzzle **puzzleArray, int puzzleCount, Puzzle *defaultPuzzles) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    char selectionChar;
    while (1) { 
        displayBanner();
        displayMenu(menuOptions_mainMenu, 6);
        fgets(buffer, BUFFER_SIZE, stdin);
        if (sscanf(buffer, "%c", &selectionChar) == 1) {
            switch (selectionChar) {
                case '1':
                    menuChoosePuzzle(puzzleArray, puzzleCount);
                    clearDisplay();
                    break;
                case '2':
                    clearDisplay();
                    printf("Sudoku solver will be implemented in version 2.0\n\n");
                    break;
                case '3':
                    clearDisplay();
                    printf("Stats will be implemented in version 2.0\n\n");
                    break;
                case 'r':
                    clearDisplay();
                    if (remove("save.bin") == 0) {
                        initDataIfNoBinary(defaultPuzzles, puzzleCount);
                        loadDataFromFile(puzzleArray, &puzzleCount);
                        clearDisplay();
                        printf("Puzzles reset successfully!\n");
                    } 
                    else {
                        clearDisplay();
                        printf("Unable to reset data\n");
                        exit(1);
                    }
                    break;
                case 'q':
                    clearDisplay();
                    saveDataToFile(*puzzleArray, puzzleCount);
                    return;
                default:
                    clearDisplay();
                    printf("Invalid input\n\n");
            }
        }
        else {
            clearDisplay();
            printf("%s\n", buffer);
            printf("Invalid input\n\n");
        }
    }
}

int main() {
    int currentID = 0; // add to read from file
    int puzzleArrayCount = 0;
    Puzzle *puzzleArray = NULL;
    Puzzle exWrong1 = {{ // 1,1 --> 7
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
    Puzzle exWrong2 = {{ // 1,2 --> 4
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 0, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 2, 8, 6, 3, 1, 9}
        }};
    Puzzle exWrong3 = {{
        {3, 1, 6, 0, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 0, 6, 8},
        {4, 8, 0, 6, 2, 0, 5, 3, 1},
        {0, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 0, 3, 0, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 0}, 
        {1, 3, 0, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 0, 4},
        {7, 0, 5, 2, 8, 6, 3, 1, 9}
        }};
    Puzzle exWrong4 = {{
        {3, 0, 6, 0, 7, 8, 4, 0, 2},
        {5, 2, 9, 1, 3, 4, 0, 0, 8},
        {4, 8, 0, 6, 2, 0, 5, 3, 1},
        {0, 0, 3, 4, 1, 5, 9, 0, 7},
        {9, 7, 4, 8, 0, 3, 0, 2, 5},
        {8, 5, 1, 7, 9, 0, 6, 0, 0}, 
        {1, 3, 0, 0, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 0, 4},
        {7, 0, 5, 2, 8, 6, 3, 1, 9}
        }};

    Puzzle defaultPuzzles[] = {exWrong1, exWrong2, exWrong3, exWrong4}; 

    initDataIfNoBinary(defaultPuzzles, (sizeof(defaultPuzzles) / sizeof(defaultPuzzles[0])));
    loadDataFromFile(&puzzleArray, &puzzleArrayCount);
    menuMain(&puzzleArray, puzzleArrayCount, defaultPuzzles);

    free(puzzleArray);

    return 0;
}
