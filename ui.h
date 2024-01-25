#ifndef UI_I
#define UI_I


#include "./dependencies.h"
#include "./puzzle.h"


/// @brief Key-value (dictionary) pair to store display strings
typedef struct {
    /// @brief Key to be converted by translate()
    char* key;
    /// @brief Value to be output to display
    char value[LINE_MAX];
} translation;


char* translate(char* key);
void clearDisplay();
void displayBanner();
void displayPuzzleGrid(Puzzle puzzle);
void displayPuzzleUserGrid(Puzzle puzzle);


void menuPlay(Puzzle *puzzle);
void menuChoosePuzzle(PuzzleArray *puzzleArray, int puzzleCount);
void menuDelete(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr);
void menuSolver(PuzzleArray *puzzleArrayPtr, int puzzleCount);
void menuStats(PuzzleArray *puzzleArray, int puzzleCount);
void menuGenerate(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr);
void menuManager(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, PuzzleArray defaultPuzzleArray, int defaultPuzzleCount);
void menuMain(PuzzleArray *puzzleArray, int *puzzleCountPtr, PuzzleArray defaultPuzzles, int defaultPuzzleCount);


#endif