#include <stdio.h>
#include <stdlib.h>

#include "global_vars.h"
#include "types.h"
#include "world.h"

WorldCell** buildWorld() {
    /* allocate the n*m contiguous items */
    WorldCell *p = (WorldCell *)malloc(R * C *sizeof(WorldCell));

    /* allocate the row pointers into the memory */
    WorldCell** mat = (WorldCell **)malloc(R * sizeof(WorldCell *));

    /* set up the pointers into the contiguous memory */
    for (int i=0; i<R; i++)
       mat[i] = &(p[i * R]);
    
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            mat[i][j].type = EMPTY;
        }
    }

    return mat;
}

void destroyWorld(WorldCell** mat) {
    /* free the memory - the first element of the array is at the start */
    free(ARRAY(mat));

    /* free the pointers into the memory */
    free(mat);

    mat = NULL;
}