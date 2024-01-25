/*
    TODO:
    - Fix sudoku generator
    - Unit tests
    - Documentation
    - Evaluate code based on criteria --> clean up and refactor
    - Video
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define BIN_SAVE_FILENAME "save.bin"
#define LOG_FILENAME "log.txt"

#define GRID_SIZE 9
#define SUBGRID_SIZE 3
#define LINE_MAX 80
#define BUFFER_SIZE 128

#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RED     "\x1b[31m"


clock_t startTime;


typedef struct {
    char* key;
    char value[LINE_MAX];
} translation; // key-value pair

// ERROR --> strings for stderr
translation dictionaryEN[] = {
    {"MENU_SELECTION", "Enter selection: "},
    {"INVALID_INPUT", "Invalid input"},

    {"MENU_MAIN_OPTION_1", "1 : Choose a puzzle to play"},
    {"MENU_MAIN_OPTION_2", "2 : Algorithmic solver"},
    {"MENU_MAIN_OPTION_3", "3 : Puzzle manager"},
    {"MENU_MAIN_OPTION_4", "4 : Statistics"},
    {"MENU_MAIN_OPTION_Q", "q : Quit the program"},

    {"MENU_MANAGER_OPTION_1", "1 : Reset everything*"},
    {"MENU_MANAGER_OPTION_2", "2 : Generate a new puzzle"},
    {"MENU_MANAGER_OPTION_3", "3 : Delete a puzzle"},
    {"MENU_MANAGER_OPTION_Q", "q : Back"},
    {"MENU_MANAGER_NOTE", "* Deletes solutions, resets to default puzzles"},
    {"MENU_MANAGER_RESET", "Puzzles reset successfully!"},

    {"MENU_DELETE_LOADED", "puzzles have been loaded"},
    {"MENU_DELETE_OPTION_N", "n : Delete nth puzzle"},
    {"MENU_DELETE_OPTION_Q", "q : Back"},
    {"MENU_DELETE_MISSING", "Puzzle does not exist"},
    {"MENU_DELETE_PUZZLE", "Puzzle"},
    {"MENU_DELETE_DELETED", "deleted successfully!"},

    {"MENU_GENERATE_LOADED", "puzzles have been loaded"},
    {"MENU_GENERATE_OPTION_N", "n : Generate puzzle with n clues"},
    {"MENU_GENERATE_OPTION_Q", "q : Back"},
    {"MENU_GENERATE_CLUES", "Puzzle must have [17;81] clues"},
    {"MENU_GENERATE_GENERATED", "Puzzle generated successfully!"},

    {"MENU_SOLVER_LOADED", "puzzles have been loaded"},
    {"MENU_SOLVER_OPTION_N", "n : Solve nth puzzle"},
    {"MENU_SOLVER_OPTION_Q", "q : Back"},
    {"MENU_SOLVER_MISSING", "Puzzle does not exist"},
    {"MENU_SOLVER_SUCCESS", "Puzzle solved successfully!"},

    {"MENU_STATS_SOLVED", "Sudokus solved:"},
    {"MENU_STATS_LAUNCHCOUNT", "Times this program was launched:"},
    {"MENU_STATS_RUNTIME", "CPU runtime this launch:"},
    {"MENU_STATS_TOTALRUNTIME", "Total CPU runtime:"},
    {"MENU_STATS_OPTION_Q", "q : Exit statistics"},

    {"MENU_CHOOSEPUZZLE_OPTION_R", "r : Play random puzzle"},
    {"MENU_CHOOSEPUZZLE_OPTION_N", "n : Play nth puzzle"},
    {"MENU_CHOOSEPUZZLE_OPTION_Q", "q : Back"},
    {"MENU_CHOOSEPUZZLE_LOADED", "puzzles loaded"},
    {"MENU_CHOOSEPUZZLE_MISSING", "Puzzle does not exist"},

    {"MENU_PLAY_OPTION_Q", "q : Back"},
    {"MENU_PLAY_OPTION_XY", "x y value : change value at (x,y)"},
    {"MENU_PLAY_OPTION_R", "r : Reset puzzle"},
    {"MENU_PLAY_SOLVED", "This puzzle has been solved"},
    {"MENU_PLAY_VALUEHIGHER", "Values cannot be higher than"},
    {"MENU_PLAY_VALUELOWER", "Values cannot be lower than"},
    {"MENU_PLAY_HINTVALUE", "Cannot change hint values!"},
    {"MENU_PLAY_VALUEAT", "Value at"},
    {"MENU_PLAY_CHANGEDTO", "changed to"},
    {"MENU_PLAY_SUCCESSFULLY", "successfully!"},
    {"MENU_PLAY_RESET", "Puzzle has been reset!"},

    {"ERROR_RESET_DATA", "Unable to reset data"},
    {"ERROR_MEMORY_ALLOCATION", "Memory allocation failure"},
    {"ERROR_OPEN_FILE", "Failed to open file"},
    {"ERROR_WRITE_PUZZLECOUNT", "Failed to write puzzleCount to file"},
    {"ERROR_WRITE_PUZZLES", "Failed to write puzzles to file"},
    {"ERROR_READ_PUZZLECOUNT", "Failed to read puzzleCount from file"},
    {"ERROR_READ_PUZZLES", "Failed to read puzzles from file"}
};

char* translate(char* key) {
    int i;
    for (i = 0; i < sizeof(dictionaryEN) / sizeof(translation); i++) {
        if (strcmp(dictionaryEN[i].key, key) == 0) {
            return dictionaryEN[i].value;
        }
    }
    fprintf(stderr, "Dictionary value not found: %s\n", key);
    exit(1);
}

typedef struct Puzzle {
    int grid[GRID_SIZE][GRID_SIZE];
    int userGrid[GRID_SIZE][GRID_SIZE]; // Modified by user, can be reset
    int map[GRID_SIZE][GRID_SIZE];
} Puzzle;

typedef Puzzle* PuzzleArray;

long double findCurrentRuntime() {
    clock_t endTime = clock();
    long double cpuTime = ((long double) (endTime - startTime)) / CLOCKS_PER_SEC;
    return cpuTime;
}

void logRuntime() {
    long double cpuTime = findCurrentRuntime();

    FILE *file = fopen(LOG_FILENAME, "a+");
    if (file == NULL) {
        printf("FAILED OPENING FILE\n");
        return;
    }
    
    fprintf(file, "Program CPU runtime: %Lf\n", cpuTime);
    fclose(file);
}

void logLaunch() {
    FILE *file = fopen(LOG_FILENAME, "a+");
    if (file == NULL) {
        printf("FAILED OPENING FILE\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char buf[BUFFER_SIZE];
    strftime(buf, sizeof(buf), "%c", t);

    fprintf(file, "Program launched at: %s\n", buf);
    fclose(file);
}

long double readTotalRuntime() {
    long double totalRuntime = 0.0, previousRuntime = 0.0;
    char line[BUFFER_SIZE];

    FILE *file = fopen(LOG_FILENAME, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return -1.0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "Program CPU runtime: %Lf\n", &previousRuntime) == 1) {
            totalRuntime += previousRuntime;
        }
    }

    totalRuntime += findCurrentRuntime();

    fclose(file);

    return totalRuntime;
}

int readLaunchCount() {
    int launchCount = 0;
    char line[BUFFER_SIZE];

    FILE *file = fopen(LOG_FILENAME, "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return -1;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, "Program launched at:", 20) == 0) {
            launchCount++;
        }
    }

    fclose(file);

    return launchCount;
}

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

// improper indentation is intentional!
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

// int checkRows(Puzzle puzzle) {
//     for (int i = 0; i < GRID_SIZE; ++i) {
//         for (int num = 1; num <= GRID_SIZE; ++num) {
//             int count = 0;
//             for (int j = 0; j < GRID_SIZE; ++j) {
//                 if (puzzle.userGrid[i][j] == num) {
//                     ++count;
//                 }
//             }
//             if (count != 1) {
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }

// int checkColumns(Puzzle puzzle) {
//     for (int j = 0; j < GRID_SIZE; ++j) {
//         for (int num = 1; num <= GRID_SIZE; ++num) {
//             int count = 0;
//             for (int i = 0; i < GRID_SIZE; ++i) {
//                 if (puzzle.userGrid[i][j] == num) {
//                     ++count;
//                 }
//             }
//             if (count != 1) {
//                 return 0;
//             }
//         }
//     }
//     return 1;
// }

// int checkBoxes(Puzzle puzzle) {
//     for (int vertPadding = 0; vertPadding <= GRID_SIZE-3; vertPadding += 3) {
//         for (int horzPadding = 0; horzPadding <= GRID_SIZE-3; horzPadding += 3) {
//             for (int num = 1; num <= GRID_SIZE; ++num) {
//                 int count = 0;
//                 for (int i = horzPadding; i < horzPadding+3; ++i) {
//                     for (int j = vertPadding; j < vertPadding+3; ++j) {
//                         if (puzzle.userGrid[i][j] == num) {
//                             ++count;
//                         }
//                     }
//                 }
//                 if (count != 1) {
//                     return 0;
//                 }
//             }
//         }
//     }
//     return 1;
// }

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

// other check functions don't work here because they check if *every* number appears exactly once
// this checks exactly one number
int isSquareSafe(Puzzle *puzzle, int row, int col, int num) {
    // Check the row
    for (int x = 0; x < GRID_SIZE; x++)
        if (puzzle->userGrid[row][x] == num)
            return 0;

    // Check the column
    for (int x = 0; x < GRID_SIZE; x++)
        if (puzzle->userGrid[x][col] == num)
            return 0;

    // Check the box
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (puzzle->userGrid[i + startRow][j + startCol] == num)
                return 0;

    return 1;
}

// recursive implementation of backtracking (brute-force) algorithm
int solveSudokuUserGrid(Puzzle *puzzle, int row, int col) {
    if (row == GRID_SIZE - 1 && col == GRID_SIZE)
        return 1;

    if (col == GRID_SIZE) {
        row++;
        col = 0;
    }

    if (puzzle->userGrid[row][col] > 0)        return solveSudokuUserGrid(puzzle, row, col + 1);

    for (int num = 1; num <= GRID_SIZE; num++) {
        if (isSquareSafe(puzzle, row, col, num)) {
            puzzle->userGrid[row][col] = num;

            if (solveSudokuUserGrid(puzzle, row, col + 1))
                return 1;
        }

        puzzle->userGrid[row][col] = 0;  // undo the current cell for backtracking
    }

    return 0;
}


int countSolvedSudokus(int puzzleArrayCount, PuzzleArray puzzleArray) {
    int solvedCount = 0;
    for (int i = 0; i < puzzleArrayCount; ++i) {
        if (isSudokuSolved(puzzleArray[i]) == 1) {
            ++solvedCount;
        }
    }
    return solvedCount;
}

void fillUserGridDiagonal(Puzzle* puzzle) {
    for (int i = 0; i < GRID_SIZE; i += SUBGRID_SIZE) {
        int numbers[GRID_SIZE];
        for (int j = 0; j < GRID_SIZE; ++j) {
            numbers[j] = j + 1;
        }

        // Shuffle using Fisher-Yates algorithm
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

    // Remove numbers until n remain
    while (clues > n) {
        int i = rand() % GRID_SIZE;
        int j = rand() % GRID_SIZE;
        if (puzzle->userGrid[i][j] != 0) {
            puzzle->userGrid[i][j] = 0;
            clues--;
        }
    }
}

void copyUserGridtoGrid(Puzzle *puzzle) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            puzzle->grid[i][j] = puzzle->userGrid[i][j];
        }
    }
}

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

// void removeLastPuzzle(Puzzle **puzzleArray, int *puzzleCount) {
//     if (*puzzleCount > 0) {
//         *puzzleArray = realloc(*puzzleArray, (*puzzleCount - 1) * sizeof(Puzzle));
//         if ((*puzzleCount - 1) > 0 && *puzzleArray == NULL) {
//             printf("Memory allocation failure\n");
//             exit(1);
//         }
//         *puzzleCount = *puzzleCount - 1;
//     }
//     else {
//         printf("Cannot remove puzzle from empty list\n");
//     }
// }

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

void saveDataToFile(Puzzle *puzzleArray, int puzzleCount) {
    FILE *file = fopen(BIN_SAVE_FILENAME, "wb");
    if (file == NULL) {
        fprintf(stderr, "%s", translate("ERROR_OPEN_FILE"));
        exit(1);
    }
    if (fwrite(&puzzleCount, sizeof(puzzleCount), 1, file) != 1) {
        fprintf(stderr, "%s", translate("ERROR_WRITE_PUZZLECOUNT"));
        exit(1);
    }
    size_t itemsWritten = fwrite(puzzleArray, sizeof(Puzzle), puzzleCount, file);
    if (itemsWritten != puzzleCount) {
        fprintf(stderr, "%s", translate("ERROR_WRITE_PUZZLES"));
        exit(1);
    }
    fclose(file);
}

void loadDataFromFile(PuzzleArray *puzzleArray, int *puzzleCount) {
    FILE *file = fopen(BIN_SAVE_FILENAME, "rb");
    if (file == NULL) {
        fprintf(stderr, "%s", translate("ERROR_OPEN_FILE"));
        exit(1);
    }

    if (fread(puzzleCount, sizeof(*puzzleCount), 1, file) != 1) {
        fprintf(stderr, "%s", translate("ERROR_READ_PUZZLECOUNT"));
        exit(1);
    }

    *puzzleArray = malloc(sizeof(Puzzle) * (*puzzleCount));
    if (*puzzleArray == NULL) {
        fprintf(stderr, "%s", translate("ERROR_MEMORY_ALLOCATION"));
        exit(1);
    }

    size_t itemsRead = fread(*puzzleArray, sizeof(Puzzle), *puzzleCount, file);
    if (itemsRead != *puzzleCount) {
        fprintf(stderr, "%s", translate("ERROR_READ_PUZZLES"));
        exit(1);
    }

    fclose(file);
}

void initDataIfNoBinary(Puzzle *puzzleArray, int puzzleCount) {
    for (int i = 0; i < puzzleCount; ++i) {
        generateUserGrid(&(puzzleArray[i]));
        generateBitmap(&(puzzleArray[i]));
    }
    FILE *file = fopen(BIN_SAVE_FILENAME, "rb");
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
            printf(ANSI_COLOR_GREEN "%s\n" ANSI_COLOR_RESET,  translate("MENU_PLAY_SOLVED"));
            if (firstLoop == 1) {
                printf("\n");
            }
        }
        
        if (firstLoop == 0) {
            printf("\n");
        }
        firstLoop = 0;

        displayPuzzleUserGrid(*puzzle);
        printf("%s\n", translate("MENU_PLAY_OPTION_Q"));
        printf("%s\n", translate("MENU_PLAY_OPTION_XY"));
        printf("%s\n\n", translate("MENU_PLAY_OPTION_R"));
        printf("%s", translate("MENU_SELECTION"));
        fgets(buffer, BUFFER_SIZE, stdin);
        printf("%d\n", val);

        if (sscanf(buffer, "%d %d %d", &x, &y, &val) == 3) {
            if (val > GRID_SIZE) {
                clearDisplay();
                printf("%s %d!\n", translate("MENU_PLAY_VALUEHIGHER"),GRID_SIZE);
            }
            else if (val < 0) {
                clearDisplay();
                printf("%s 0!\n", translate("MENU_PLAY_VALUELOWER"));
            }
            
            else if (changeValue(puzzle, x, y, val) == -1) {
                clearDisplay();
                printf("%s\n", translate("MENU_PLAY_HINTVALUE"));
            }
            else {
                clearDisplay();
                printf("%s (%i, %i) %s %i %s\n", translate("MENU_PLAY_VALUEAT"), \
                x, y, translate("MENU_PLAY_CHANGEDTO"), val, translate("MENU_PLAY_SUCCESSFULLY"));
            }
        }
        else if (buffer[0] == 'r') {
            generateUserGrid(puzzle);
            clearDisplay();
            printf(ANSI_COLOR_RED "%s\n" ANSI_COLOR_RESET, translate("MENU_PLAY_RESET"));
        }
        else if (buffer[0] == 'q') {
            clearDisplay();
            break;
        }
        else {
            clearDisplay();
            printf("%s\n", translate("INVALID_INPUT"));
        }
    }
}

void menuChoosePuzzle(PuzzleArray *puzzleArray, int puzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int selection;
    char selectionChar;
    while (1) { 
        displayBanner();
        printf("%d %s\n\n", puzzleCount, translate("MENU_CHOOSEPUZZLE_LOADED"));
        printf("%s\n", translate("MENU_CHOOSEPUZZLE_OPTION_R"));
        printf("%s\n", translate("MENU_CHOOSEPUZZLE_OPTION_N"));
        printf("%s\n\n", translate("MENU_CHOOSEPUZZLE_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection > 0 && selection <= puzzleCount) {
                menuPlay(&(*puzzleArray)[selection-1]);
            }
            else {
                clearDisplay();
                printf("%s\n\n", translate("MENU_CHOOSEPUZZLE_MISSING"));
            }
        }
        else if (sscanf(buffer, "%c", &selectionChar) == 1) {
            if (selectionChar == 'r') {
                menuPlay(&(*puzzleArray)[rand()%puzzleCount]);
            }
            else if (selectionChar == 'q') {
                clearDisplay();
                break;
            }
            else {
                clearDisplay();
                printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n", translate("INVALID_INPUT"));
        }
    }
    
    
}

void menuDelete(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int selection;
    char selectionChar;
    while (1) { 
        displayBanner();
        printf("%d %s\n\n", *puzzleCountPtr, translate("MENU_DELETE_LOADED"));
        printf("%s\n", translate("MENU_DELETE_OPTION_N"));
        printf("%s\n\n", translate("MENU_DELETE_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection > 0 && selection <= *puzzleCountPtr) {
                deleteNthPuzzle(puzzleArrayPtr, puzzleCountPtr, (selection-1));
                clearDisplay();
                printf(ANSI_COLOR_GREEN "%s %d %s\n\n" ANSI_COLOR_RESET, translate("MENU_DELETE_PUZZLE"), \
                selection, translate("MENU_DELETE_DELETED"));
            }
            else {
                clearDisplay();
                printf("%s\n\n", translate("MENU_DELETE_MISSING"));
            }
        }
        else if (sscanf(buffer, "%c", &selectionChar) == 1) {
            if (selectionChar == 'q') {
                clearDisplay();
                break;
            }
            else {
                clearDisplay();
                printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n", translate("INVALID_INPUT"));
        }
    }
}

void menuSolver(PuzzleArray *puzzleArrayPtr, int puzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int selection;
    char selectionChar;
    while (1) { 
        displayBanner();
        printf("%d %s\n\n", puzzleCount, translate("MENU_SOLVER_LOADED"));
        printf("%s\n", translate("MENU_SOLVER_OPTION_N"));
        printf("%s\n\n", translate("MENU_SOLVER_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection > 0 && selection <= puzzleCount) {
                solveSudokuUserGrid(&(*puzzleArrayPtr)[selection-1], 0, 0);
                clearDisplay();
                printf(ANSI_COLOR_GREEN "%s\n\n" ANSI_COLOR_RESET, translate("MENU_SOLVER_SUCCESS"));
            }
            else {
                clearDisplay();
                printf("%s\n\n", translate("MENU_SOLVER_MISSING"));
            }
        }
        else if (sscanf(buffer, "%c", &selectionChar) == 1) {
            if (selectionChar == 'q') {
                clearDisplay();
                break;
            }
            else {
                clearDisplay();
                printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n", translate("INVALID_INPUT"));
        }
    }
}

void menuStats(PuzzleArray *puzzleArray, int puzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    char selectionChar;

    int solvedCount = countSolvedSudokus(puzzleCount, *puzzleArray);

    while (1) { 
        displayBanner();
        
        printf("%s %d/%d (%d%%)\n", translate("MENU_STATS_SOLVED"), solvedCount, puzzleCount, \
        (int)((double)solvedCount/puzzleCount * 100));
        printf("%s %d\n", translate("MENU_STATS_LAUNCHCOUNT"), readLaunchCount());
        printf("%s %Lfs\n", translate("MENU_STATS_RUNTIME"), findCurrentRuntime());
        printf("%s %Lfs\n\n", translate("MENU_STATS_TOTALRUNTIME"), readTotalRuntime());
        printf("%s\n\n", translate("MENU_STATS_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%c", &selectionChar) == 1) {
            switch (selectionChar) {
                case 'q':
                    clearDisplay();
                    return;
                default:
                    clearDisplay();
                    printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n\n", translate("INVALID_INPUT"));
        }
    }
}

void menuGenerate(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    int selection;
    char selectionChar;
    while (1) { 
        displayBanner();
        printf("%d %s\n\n", *puzzleCountPtr, translate("MENU_GENERATE_LOADED"));
        printf("%s\n", translate("MENU_GENERATE_OPTION_N"));
        printf("%s\n\n", translate("MENU_GENERATE_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%d", &selection) == 1) {
            if (selection >= 17 && selection <= 81) {
                printf("%d %d", *puzzleCountPtr, selection);
                generatePuzzle(puzzleArrayPtr, puzzleCountPtr, selection);
                clearDisplay();
                printf(ANSI_COLOR_GREEN "%s\n\n" ANSI_COLOR_RESET, translate("MENU_GENERATE_GENERATED"));
            }
            else {
                clearDisplay();
                printf("%s\n\n", translate("MENU_GENERATE_CLUES"));
            }
        }
        else if (sscanf(buffer, "%c", &selectionChar) == 1) {
            if (selectionChar == 'q') {
                clearDisplay();
                break;
            }
            else {
                clearDisplay();
                printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n", translate("INVALID_INPUT"));
        }
    }
}

void menuManager(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, PuzzleArray defaultPuzzleArray, int defaultPuzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    char selectionChar;

    while (1) { 
        displayBanner();
        printf("%s\n", translate("MENU_MANAGER_OPTION_1"));
        printf("%s\n", translate("MENU_MANAGER_OPTION_2"));
        printf("%s\n", translate("MENU_MANAGER_OPTION_3"));
        printf("%s\n\n", translate("MENU_MANAGER_OPTION_Q"));
        printf("%s\n\n", translate("MENU_MANAGER_NOTE"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%c", &selectionChar) == 1) {
            switch (selectionChar) {
                case '1':
                    clearDisplay();
                    if (remove(BIN_SAVE_FILENAME) == 0) {
                        initDataIfNoBinary(defaultPuzzleArray, defaultPuzzleCount);
                        loadDataFromFile(puzzleArrayPtr, puzzleCountPtr);
                        clearDisplay();
                        printf("%s\n", translate("MENU_MANAGER_RESET"));
                    } 
                    else {
                        clearDisplay();
                        fprintf(stderr, "%s\n", translate("ERROR_RESET_DATA"));
                        exit(1);
                    }
                    break;
                case '2':
                    clearDisplay();
                    menuGenerate(puzzleArrayPtr, puzzleCountPtr);
                    break;
                case '3':
                    clearDisplay();
                    menuDelete(puzzleArrayPtr, puzzleCountPtr);
                    break;
                case 'q':
                    clearDisplay();
                    return;
                default:
                    clearDisplay();
                    printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n\n", translate("INVALID_INPUT"));
        }
    }

}

void menuMain(PuzzleArray *puzzleArray, int *puzzleCountPtr, PuzzleArray defaultPuzzles, int defaultPuzzleCount) {
    clearDisplay();
    char buffer[BUFFER_SIZE];
    char selectionChar;

    while (1) { 
        displayBanner();
        printf("%s\n", translate("MENU_MAIN_OPTION_1"));
        printf("%s\n", translate("MENU_MAIN_OPTION_2"));
        printf("%s\n", translate("MENU_MAIN_OPTION_3"));
        printf("%s\n", translate("MENU_MAIN_OPTION_4"));
        printf("%s\n\n", translate("MENU_MAIN_OPTION_Q"));
        printf("%s", translate("MENU_SELECTION"));

        fgets(buffer, BUFFER_SIZE, stdin);

        if (sscanf(buffer, "%c", &selectionChar) == 1) {
            switch (selectionChar) {
                case '1':
                    menuChoosePuzzle(puzzleArray, *puzzleCountPtr);
                    clearDisplay();
                    break;
                case '2':
                    clearDisplay();
                    menuSolver(puzzleArray, *puzzleCountPtr);
                    break;
                case '3':
                    menuManager(puzzleArray, puzzleCountPtr, defaultPuzzles, defaultPuzzleCount);
                    clearDisplay();
                    break;
                case '4':
                    menuStats(puzzleArray, *puzzleCountPtr);
                    clearDisplay();
                    break;
                case 'q':
                    clearDisplay(); 
                    saveDataToFile(*puzzleArray, *puzzleCountPtr);
                    return;
                default:
                    clearDisplay();
                    printf("%s\n", translate("INVALID_INPUT"));
            }
        }
        else {
            clearDisplay();
            printf("%s\n\n", translate("INVALID_INPUT"));
        }
    }
}

int main() {
    startTime = clock();
    logLaunch();
    atexit(logRuntime);

    srand(time(NULL));


    
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
    int defaultPuzzleCount = (sizeof(defaultPuzzles) / sizeof(defaultPuzzles[0]));

    initDataIfNoBinary(defaultPuzzles, defaultPuzzleCount);
    loadDataFromFile(&puzzleArray, &puzzleArrayCount);
    menuMain(&puzzleArray, &puzzleArrayCount, defaultPuzzles, defaultPuzzleCount);

    // printf("%i\n", countSolvedSudokus(puzzleArrayCount, puzzleArray));

    free(puzzleArray);

    // printf("%Lf\n", readTotalRuntime());
    // printf("%d\n", readLaunchCount());

    return 0;
}
