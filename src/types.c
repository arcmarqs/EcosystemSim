#include <stdlib.h>

#include "types.h"

Type getType(int v) {
    switch(v) {
        case 0: return EMPTY;
        case 1: return ROCK;
        case 2: return FOX;
        case 3: return RABBIT;
        default: exit(1);
    }
}

char* getFullName(Type t) {
    switch(t) {
        case ROCK: return "ROCK";
        case FOX: return "FOX";
        case RABBIT: return "RABBIT";
        default: exit(1);
    }
}

char getMinName(Type t) {
    switch(t) {
        case ROCK: return '*';
        case FOX: return 'F';
        case RABBIT: return 'R';
        case EMPTY: return ' ';
        default: exit(1);
    }
}
