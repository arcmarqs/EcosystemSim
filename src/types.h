#ifndef TYPES
#define TYPES

typedef enum _Type {
    EMPTY = 0,
    ROCK = 1,
    FOX = 2,
    RABBIT = 3
} Type;

typedef struct _WorldCell {
    Type type;
    int currentGenProc;  // Current proceation age
    int currentGenFood;  //Generations since the fox has eaten a rabbit
} WorldCell;

Type getType(int v);
char* getFullName(Type t);
char getMinName(Type t);

#endif