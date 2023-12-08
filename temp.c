assert(isSudokuSolved(exSolved) == true);
assert(isSudokuSolved(exWrong1) == false);
assert(isSudokuSolved(exWrong1) == false);
assert(isSudokuSolved(exHas0) == false);
assert(isSudokuSolved(exWrong917) == false);
generateBitmap(&exWrong917);
// outputPuzzle(exWrong917);
changeValue(&exWrong917, 1, 1, 7);
assert(isSudokuSolved(exWrong917) == true);
// outputPuzzle(exWrong917);
generateBitmap(&exHas0);
outputPuzzle(exHas0);
changeValue(&exHas0, 2, 4, 9);
outputPuzzle(exHas0);

Puzzle exWrong1 = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 5, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 3, 8, 6, 3, 1, 9}
        }};

    Puzzle exWrong2 = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 9},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 1, 2, 8, 6, 3, 1, 9}
        }};

    Puzzle exHas0 = {{
        {3, 1, 0, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 0, 3, 4, 7, 6, 8},
        {4, 8, 0, 6, 2, 0, 5, 0, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 0, 2, 5},
        {8, 0, 0, 7, 9, 0, 6, 4, 3}, 
        {1, 3, 0, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
        }};

Puzzle exSolved = {{
        {3, 1, 6, 5, 7, 8, 4, 9, 2},
        {5, 2, 9, 1, 3, 4, 7, 6, 8},
        {4, 8, 7, 6, 2, 9, 5, 3, 1},
        {2, 6, 3, 4, 1, 5, 9, 8, 7},
        {9, 7, 4, 8, 6, 3, 1, 2, 5},
        {8, 5, 1, 7, 9, 2, 6, 4, 3}, 
        {1, 3, 8, 9, 4, 7, 2, 5, 6},
        {6, 9, 2, 3, 5, 1, 8, 7, 4},
        {7, 4, 5, 2, 8, 6, 3, 1, 9}
        }};
    