#include "logger.h"

FILE *fptr = NULL;  // Define the global file pointer

void openFile(const char *filename, const char *mode) {
    fptr = fopen(filename, mode);
    if (!fptr) {
        perror("Failed to open file");
    }
}

void closeFile() {
    if (fptr) {
        fclose(fptr);
        fptr = NULL;
    }
}
