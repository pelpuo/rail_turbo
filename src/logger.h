#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

extern FILE *fptr;

void openFile(const char *filename, const char *mode);
void closeFile();

#endif