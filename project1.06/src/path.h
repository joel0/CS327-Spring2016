//
// Created by joelm on 2016-02-07.
//

#ifndef PROJECT_PATH_H
#define PROJECT_PATH_H

#include <stdint.h>
#include "dungeon.h"

#define PATH_DIST_INFINITE 0xFF

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t dist_t;

int pathMallocDistGrid(dist_t ***gridPtr);
void pathFreeDistGrid(dist_t **gridPtr);
int pathTunneling(dungeon_t *dungeonPtr);
int pathNontunneling(dungeon_t *dungeonPtr);
void printDistGrid(dungeon_t *dungeonPtr, uint8_t **grid);

#ifdef __cplusplus
}
#endif

#endif //PROJECT_PATH_H
