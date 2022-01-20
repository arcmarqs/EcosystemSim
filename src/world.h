#ifndef MATRIX
#define MATRIX

#include "global_vars.h"

#define ARRAY(MAT) &(MAT[0][0])

WorldCell** buildWorld();
void destroyWorld(WorldCell** mat);

#endif