#ifndef FILES_H
#define FILES_H


#include "puzzle.h"
#include "./dependencies.h"


extern clock_t startTime;


long double findCurrentRuntime();
void logRuntime();
void logLaunch();
long double readTotalRuntime();
int readLaunchCount();


void saveDataToFile(Puzzle *puzzleArray, int puzzleCount);
void loadDataFromFile(PuzzleArray *puzzleArray, int *puzzleCount);
void initDataIfNoBinary(Puzzle *puzzleArray, int puzzleCount);


#endif