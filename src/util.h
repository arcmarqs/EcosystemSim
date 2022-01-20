#ifndef UTIL
#define UTIL

void readInput();
void writeOutput();

void saveGeneration();

void decodePos(int p, int i, int j, int* x, int* y);
int calculateDirection(int i, int j, int* p, int psum);

void writeState();
void writeStateAll();

#endif