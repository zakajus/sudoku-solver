/**
 * @file dependencies.h
 * @author Kajus Zakaras (kajus.z@tuta.io)
 * @brief Includes library dependencies, defines macros
 * @version 1.00
 * @date 2024-01-25
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DEPENDENCIES_H
#define DEPENDENCIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


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

#endif