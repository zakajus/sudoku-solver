#include "./dependencies.h"
#include "./ui.h"
#include "./puzzle.h"
#include "./files.h"


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

/// @brief Clears CLI display
void clearDisplay() {
    printf("\033[H\033[J");
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
