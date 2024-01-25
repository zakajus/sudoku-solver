#include "./dependencies.h"
#include "./puzzle.h"
#include "./ui.h"


/// @brief Global variable to track CPU runtime
clock_t startTime;



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
