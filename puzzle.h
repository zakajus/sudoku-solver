/**
 * @file puzzle.h
 * @author Kajus Zakaras (kajus.z@tuta.io)
 * @brief Header file for puzzle.h
 * @version 1.00
 * @date 2024-01-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PUZZLE_H
#define PUZZLE_H

#include "dependencies.h"


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


/// @brief Alias for an array of Puzzle 
typedef Puzzle* PuzzleArray;


void generateBitmap(Puzzle *puzzle);
void generateUserGrid(Puzzle *puzzle);
int changeValue(Puzzle *puzzle, int x, int y, int value);
void addPuzzle(Puzzle puzzle, PuzzleArray *puzzleArray, int *puzzleCount);
int checkRow(Puzzle puzzle, int row);
int checkColumn(Puzzle puzzle, int col);
int checkBox(Puzzle puzzle, int startRow, int startCol);
int isSudokuSolved(Puzzle puzzle);
int isSquareSafe(Puzzle *puzzle, int row, int col, int num);
int solveSudokuUserGrid(Puzzle *puzzle, int row, int col);
int countSolvedSudokus(int puzzleArrayCount, PuzzleArray puzzleArray);
void fillUserGridDiagonal(Puzzle* puzzle);
void setNCluesInUserGrid(Puzzle* puzzle, int n);
void copyUserGridtoGrid(Puzzle *puzzle);
void generatePuzzle(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, int clues);
void deleteNthPuzzle(PuzzleArray *puzzleArrayPtr, int *puzzleCountPtr, int n);


#endif
