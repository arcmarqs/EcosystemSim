#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#include "global_vars.h"
#include "types.h"
#include "util.h"
#include "world.h"

#define NTHREADS 4

omp_lock_t*** locks;

int bounds[NTHREADS][2];

void initBounds() {
    int chunk = C / NTHREADS;
    int rest = C % NTHREADS;
    
    int counter = 0;
    for(int i=0; i<NTHREADS; i++) {
        bounds[i][0] = counter;
        counter += chunk;
        if(rest > 0) {
            counter++;
            rest--;
        }
        bounds[i][1] = counter - 1;
    }
}

void buildLocks() {
    locks = (omp_lock_t ***)malloc(NTHREADS * sizeof(omp_lock_t**));
    for(int i=0; i<NTHREADS; i++) {
        locks[i] = (omp_lock_t **)malloc(2 * sizeof(omp_lock_t*));
        for(int j=0; j<2; j++) {
            locks[i][j] = (omp_lock_t *)malloc(R * sizeof(omp_lock_t));
            for(int k=0; k<R; k++) {
                omp_init_lock(&locks[i][j][k]);
            }
        }
    }
}

void destroyLocks() {
    for(int i=0; i<NTHREADS; i++) {
        for(int j=0; j<2; j++) {
            for(int k=0; k<R; k++) {
                omp_destroy_lock(&locks[i][j][k]);
            }
            free(locks[i][j]);
        }
        free(locks[i]);
    }
    free(locks);
    locks = NULL;
}

void enableLocks(int px, int py) {
    int tid = omp_get_thread_num();
    
    if(px <= 0 || px >= C-1)
        return;
    
    if(tid-1 >= 0 && bounds[tid-1][0] == px) {
        omp_set_lock(&locks[tid-1][0][py]);
    } else if(tid+1 < C && bounds[tid+1][1] == px) {
        omp_set_lock(&locks[tid+1][1][py]);
    }
}

void disableLocks(int px, int py) {
    int tid = omp_get_thread_num();
    
    if(px <= 0 || px >= C-1)
        return;
    
    if(tid-1 >= 0 && bounds[tid-1][0] == px) {
        omp_unset_lock(&locks[tid-1][0][py]);
    } else if(tid+1 < C && bounds[tid+1][1] == px) {
        omp_unset_lock(&locks[tid+1][1][py]);
    }
}

int canMoveFoxEmpty(int i, int j) {
    if ((i >= 0) && (j >= 0) && (i < C) && (j < R)) {
        return (oldWorld[i][j].type == RABBIT || oldWorld[i][j].type == EMPTY);
    }
    return 0;
}

int canMoveFoxEmptyFill(int i, int j, int* p) {
    p[0] = canMoveFoxEmpty(i - 1, j);
    p[1] = canMoveFoxEmpty(i, j + 1);
    p[2] = canMoveFoxEmpty(i + 1, j);
    p[3] = canMoveFoxEmpty(i, j - 1);
    return p[0] + p[1] + p[2] + p[3];
}

int canMoveFoxRabbit(int i, int j) {
    if ((i >= 0) && (j >= 0) && (i < C) && (j < R)) {
        return (newWorld[i][j].type == RABBIT);
    }
    return 0;
}

int canMoveFoxRabbitFill(int i, int j, int* p) {
    p[0] = canMoveFoxRabbit(i - 1, j);
    p[1] = canMoveFoxRabbit(i, j + 1);
    p[2] = canMoveFoxRabbit(i + 1, j);
    p[3] = canMoveFoxRabbit(i, j - 1);
    return p[0] + p[1] + p[2] + p[3];
}

int procreateFox(int sx, int sy) {
    if (oldWorld[sx][sy].currentGenProc >= GEN_PROC_FOXES) {
        newWorld[sx][sy].type = oldWorld[sx][sy].type;
        // newWorld[sx][sy].currentGenFood = 0;
        newWorld[sx][sy].currentGenProc = 0;
        return 1;
    }
    return 0;
}

void moveFoxTo(int sx, int sy, int direction) {
    // this fuction is called with is possible to move

    int dx, dy;
    int proc = 0;
    decodePos(direction, sx, sy, &dx, &dy);

    enableLocks(dx,dy);

    int die = 0;
    if (newWorld[dx][dy].type == RABBIT) {
        proc = procreateFox(sx, sy);
        newWorld[dx][dy].currentGenFood = 0;
        newWorld[dx][dy].currentGenProc = oldWorld[sx][sy].currentGenProc + 1;
    } else if (oldWorld[sx][sy].currentGenFood >= GEN_FOOD_FOXES - 1) {
        die = 1;
    } else if (newWorld[dx][dy].type == FOX) {
        proc = procreateFox(sx, sy);
        int procs = 0;
        // only colliding with FOX
        if (!proc) {
            procs = oldWorld[sx][sy].currentGenProc;
        }
        if (procs > newWorld[dx][dy].currentGenProc) {
            newWorld[dx][dy].currentGenProc = oldWorld[sx][sy].currentGenProc + 1;
            if (newWorld[dx][dy].currentGenFood == 0) {
                newWorld[dx][dy].currentGenFood = 0;
            } else {
                newWorld[dx][dy].currentGenFood = oldWorld[sx][sy].currentGenFood + 1;
            }
        } else if (oldWorld[sx][sy].currentGenProc == newWorld[dx][dy].currentGenProc - 1) {
            //if this fox is less hungry
            if (oldWorld[sx][sy].currentGenFood < newWorld[dx][dy].currentGenFood) {
                newWorld[dx][dy].currentGenFood = oldWorld[sx][sy].currentGenFood + 1;
            }
        }
    } else {
        proc = procreateFox(sx, sy);
        newWorld[dx][dy].currentGenProc = oldWorld[sx][sy].currentGenProc + 1;
        newWorld[dx][dy].currentGenFood = oldWorld[sx][sy].currentGenFood + 1;
    }

    if (proc) {
        newWorld[dx][dy].currentGenProc = 0;
    }
    
    if(!die)
        newWorld[dx][dy].type = oldWorld[sx][sy].type;

    disableLocks(dx,dy);
}

void moveFox(int i, int j) {
    //search for rabbits
    int p[4];
    int psum = canMoveFoxRabbitFill(i, j, p);

    // If there are no adjacent rabbits
    if (psum == 0) {
        //search for empty spaces

        psum = canMoveFoxEmptyFill(i, j, p);
        // NO AVALIABLE MOVES
        if (psum == 0) {
            if (oldWorld[i][j].currentGenFood >= GEN_FOOD_FOXES - 1) return;  //fox dies
            newWorld[i][j].type = oldWorld[i][j].type;
            newWorld[i][j].currentGenProc = oldWorld[i][j].currentGenProc + 1;
            newWorld[i][j].currentGenFood = oldWorld[i][j].currentGenFood + 1;
            return;  // doesnt move
        }
    }
    moveFoxTo(i, j, calculateDirection(i, j, p, psum));
}

void genNextGenerationFox() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        for (int i = bounds[tid][0]; i <= bounds[tid][1]; i++) {
            for (int j = 0; j < R; j++) {
                if (oldWorld[i][j].type == FOX) {
                    moveFox(i, j);
                }
            }
        }
    }
}

int canMoveRabbit(int i, int j) {
    if ((i >= 0) && (j >= 0) && (i < C) && (j < R)) {
        return (oldWorld[i][j].type == EMPTY);
    }
    return 0;
}

int canMoveRabbitFill(int i, int j, int* p) {
    p[0] = canMoveRabbit(i - 1, j);
    p[1] = canMoveRabbit(i, j + 1);
    p[2] = canMoveRabbit(i + 1, j);
    p[3] = canMoveRabbit(i, j - 1);
    return p[0] + p[1] + p[2] + p[3];
}

int procreateRabbit(int sx, int sy) {
    if (oldWorld[sx][sy].currentGenProc >= GEN_PROC_RABBITS) {
        newWorld[sx][sy].type = oldWorld[sx][sy].type;
        newWorld[sx][sy].currentGenProc = 0;
        return 1;
    }
    return 0;
}

void moveRabbitTo(int sx, int sy, int direction) {
    // this fuction is called with is possible to move

    int dx, dy;
    decodePos(direction, sx, sy, &dx, &dy);

    int proc = procreateRabbit(sx, sy);  // 1 if procreated 0 if not

    enableLocks(dx,dy);

    if (newWorld[dx][dy].type != EMPTY) {
        // only colliding with rabbit
        if (oldWorld[sx][sy].currentGenProc > newWorld[dx][dy].currentGenProc - 1) {
            newWorld[dx][dy].currentGenProc = oldWorld[sx][sy].currentGenProc + 1;
        }
    } else {
        newWorld[dx][dy].currentGenProc = oldWorld[sx][sy].currentGenProc + 1;
    }

    if (proc) {
        newWorld[dx][dy].currentGenProc = 0; 
    }

    newWorld[dx][dy].type = oldWorld[sx][sy].type;
    
    disableLocks(dx,dy);
}

void moveRabbit(int i, int j) {
    int p[4];
    int psum = canMoveRabbitFill(i, j, p);

    if (psum == 0) {
        newWorld[i][j].type = oldWorld[i][j].type;
        newWorld[i][j].currentGenProc = oldWorld[i][j].currentGenProc + 1;
    } else {
        int direction = calculateDirection(i, j, p, psum);
        moveRabbitTo(i, j, direction);
    }
}

void genNextGenerationRabbit() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        for (int i = bounds[tid][0]; i <= bounds[tid][1]; i++) {
            for (int j = 0; j < R; j++) {
                if (oldWorld[i][j].type == RABBIT) {
                    moveRabbit(i, j);
                }
            }
        }
    }
}

int main() {
    readInput();

#ifdef TIME
    double start = omp_get_wtime();
#endif
    omp_set_num_threads(NTHREADS);

    initBounds();
    buildLocks();

    for (ACTUAL_GEN = 0; ACTUAL_GEN < N_GEN; ACTUAL_GEN++) {
        genNextGenerationRabbit();
        genNextGenerationFox();

        saveGeneration(); 
    }

    destroyLocks();

#ifdef TIME
    double finish = omp_get_wtime();
#endif  
    
    writeOutput();

#ifdef TIME
    printf("Execution time: %f seconds\n", finish - start);
#endif
}