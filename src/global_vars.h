#ifndef GLOBALVARS
#define GLOBALVARS

#include "types.h"

extern int GEN_PROC_RABBITS;  // number of generations until a rabbit can procreate

extern int GEN_PROC_FOXES;  // number of generations until a fox can procreate
extern int GEN_FOOD_FOXES;  // number of generations for a fox to die of starvation

extern int N_GEN;  // number of generations for the simulation

extern int R;  // number of rows of the matrix representing the ecosystem
extern int C;  // number of columns of the matrix representing the ecosystem

extern int N;  // number of Objects

extern int ACTUAL_GEN;

extern WorldCell** newWorld;
extern WorldCell** oldWorld;

#endif