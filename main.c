/**
 * @file main.c
 * @author Kajus Zakaras (kajus.z@tuta.io)
 * @brief Handles puzzle initialization / saving, running log files, and launching the game loop
 * @version 1.00
 * @date 2024-01-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <assert.h>
// #include <ctype.h>
// #include <math.h>


/// @brief Binary save file name
#define BIN_SAVE_FILENAME "save.bin"
/// @brief Log text file name 
#define LOG_FILENAME "log.txt"


/// @brief Grid size of Sudoku puzzle (9x9 is standard)
#define GRID_SIZE 9
/// @brief Subgrids the Sudoku grid is divided into
#define SUBGRID_SIZE 3
/// @brief Maximum length of displeyed strings
#define LINE_MAX 80
/// @brief Buffer size for user input
#define BUFFER_SIZE 128


/// @brief ASCII break code to make text magenta
#define ANSI_COLOR_MAGENTA "\x1b[35m"
/// @brief ASCII break code to make text green
#define ANSI_COLOR_GREEN   "\x1b[32m"
/// @brief ASCII break code to make text red
#define ANSI_COLOR_RED     "\x1b[31m"
/// @brief ASCII break code to reset text color
#define ANSI_COLOR_RESET   "\x1b[0m"


/// @brief Global variable to track CPU runtime
clock_t startTime;


/// @brief Key-value (dictionary) pair to store display strings
typedef struct {
    /// @brief Key to be converted by translate()
    char* key;
    /// @brief Value to be output to display
    char value[LINE_MAX];
} translation;


/// @brief English key-value dictionary for display strings
/// @note ERROR prefix strings are used in stderr stream
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

/// @brief Used to translate a display string key into string value from a localized dictionary
/// @note English dictionary (dictionaryEN) is hard-coded for now, however replacing it can be easily done using a macro
/// @param key Display string key to translate
/// @return Localized string for display output
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

/// @brief Structure to store data of a single Sudoku puzzle
/// @details The grid is always fixed, except for when generating a new sudoku
typedef struct Puzzle {
    /// @brief The unmodified version of the puzzle
    int grid[GRID_SIZE][GRID_SIZE];
    /// @brief The user modified version of the grid
    int userGrid[GRID_SIZE][GRID_SIZE];
    /// @brief Bitmap used to determine which squares are modifiable by user
    /// @note If 1 == unmodifiable
    int map[GRID_SIZE][GRID_SIZE];
} Puzzle;

/// @brief Alias for an array of puzzles 
typedef Puzzle* PuzzleArray;

/// @brief Finds the CPU runtime of the current launch
/// @return Runtime in seconds 
long double findCurrentRuntime() {
    clock_t endTime = clock();
    long double cpuTime = ((long double) (endTime - startTime)) / CLOCKS_PER_SEC;
    return cpuTime;
}

/// @brief Calculates and appends CPU runtime during this launch to log file 
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

/// @brief Appends launch date and time to log file
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

/// @brief Calculates total CPU runtime from log and the current launch
/// @return Total CPU runtime in seconds
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

/// @brief Reads total launch count from log file
/// @return Count of total launches by program
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

/// @brief Clears CLI display
void clearDisplay() {
    printf("\033[H\033[J");
}

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

/// @brief Display Sudoku banner
/// @note Improper tab indentation is intentional - otherwise banner deforms
void displayBanner() {
printf("\
  ___         _     _        \n \
/ __|_  _ __| |___| |___  _ \n \
\\__ \\ || / _` / _ \\ / / || |\n \
|___/\\_,_\\__,_\\___/_\\_\\_,_|\n \
-----------------------------\n\
");
}

/// @brief Displays grid of puzzle in CLI
/// @param puzzle The puzzle to display
/// @note As of version 1.00, never used in production, however very useful for debugging
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

/// @brief Displays user grid of puzzle in CLI
/// @param puzzle The puzzle to display
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

/// @brief Saves puzzle array and size to .bin file 
/// @param puzzleArray Array to save
/// @param puzzleCount Array size to save
/// @details Change binary file name using #BIN_SAVE_FILENAME macro
/// @note Saves array size before array!
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

/// @brief Loads puzzle array and size from .bin file, allocates memory
/// @param puzzleArray Array to load to 
/// @param puzzleCount Where to save array size
/// @details Change binary file name using #BIN_SAVE_FILENAME macro. Loads array size before array.
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


/// @brief Initializes dynamic puzzle array if there is no .bin save file
/// @param puzzleArray Array of default puzzles to initialize
/// @param puzzleCount Array size
/// @details Change binary file name using #BIN_SAVE_FILENAME macro.
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

/// @brief Opens CLI for playing a puzzle
/// @param puzzle Puzzle to play
/// @details Launched by menuChoosePuzzle()
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

/// @brief Opens CLI to choose puzzle to play
/// @param puzzleArray Array to choose puzzle from
/// @param puzzleCount Array size
/// @details Launched by menuMain(), launches menuPlay() after puzzle is chosen
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

/// @brief Opens CLI to delete a puzzle from array
/// @param puzzleArrayPtr Array to delete from
/// @param puzzleCountPtr Array size
/// @details Launched by menuManager() 
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

/// @brief Opens CLI for algorithmic solver 
/// @param puzzleArrayPtr Array to solve from
/// @param puzzleCount Array size
/// @details Launched by menuMain()
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

/// @brief Opens CLI to show statistics
/// @param puzzleArray Array to show statistic from
/// @param puzzleCount Array size
/// @details Launched by menuMain()
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

/// @brief Opens CLI to generate a new puzzle
/// @param puzzleArrayPtr Array to generate puzzle to
/// @param puzzleCountPtr Array size
/// @details Launched by menuManager()
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

/// @brief Opens CLI to reset data, navigate to menus to generate or delete puzzles
/// @param puzzleArrayPtr Puzzle array to play from
/// @param puzzleCountPtr Puzzle array size
/// @param defaultPuzzleArray Default puzzle array to reset save to
/// @param defaultPuzzleCount Default puzzle array size
/// @details Launched by menuManager(), can launch menuGenerate(), menuDelete()
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

/// @brief Opens main CLI menu when program is launched, used to navigate to all other menus
/// @param puzzleArray Puzzle array to play from
/// @param puzzleCountPtr Puzzle array size
/// @param defaultPuzzles Default puzzle array to reset save to
/// @param defaultPuzzleCount Default puzzle array size
/// @details Launched by main()
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

    srand(time(NULL)); // seed for random values

    int puzzleArrayCount = 0;
    Puzzle *puzzleArray = NULL;

    // Default puzzles to initialize with
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

    free(puzzleArray);

    return 0;
}
