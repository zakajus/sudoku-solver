/**
 * @file main.c
 * @author Kajus Zakaras (kajus.z@tuta.io)
 * @brief Handles launching the program, defining default puzzles
 * @version 1.00
 * @date 2024-01-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "./dependencies.h"
#include "./puzzle.h"
#include "./files.h"
#include "./ui.h"


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
    Puzzle exWrong2 = {{ 
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
