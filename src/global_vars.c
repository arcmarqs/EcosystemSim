#include "global_vars.h"

int GEN_PROC_RABBITS;  // number of generations until a rabbit can procreate

int GEN_PROC_FOXES;  // number of generations until a fox can procreate
int GEN_FOOD_FOXES;  // number of generations for a fox to die of starvation

int N_GEN;  // number of generations for the simulation

int R;  // number of rows of the matrix representing the ecosystem
int C;  // number of columns of the matrix representing the ecosystem

int N;  // number of Objects

int ACTUAL_GEN = 0;

WorldCell** newWorld;
WorldCell** oldWorld;
