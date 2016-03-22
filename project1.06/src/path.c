//
// Created by joelm on 2016-02-07.
//

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

#include "path.h"
#include "globals.h"
#include "utils.h"

typedef struct gridNode_struct {
    int x;
    int y;
    uint8_t* weightPtr;
} gridNode_t;

int isBorder(int x, int y);
int pathPopulate(dungeon_t* dungeonPtr, uint8_t** distGrid, uint8_t (*relDist)(dungeon_t*, int x, int y));
gridNode_t* mallocGridNode(dist_t* weightPtr, int x, int y);
int32_t pathCmp(const void *keyPtr, const void *withPtr);
void pathDataDelete(void* data);
dist_t pathCalculateWeight(uint8_t hardness);
dist_t pathCalculateRelWeightTunneling(dungeon_t* dungeonPtr, int x, int y);
dist_t pathCalculateRelWeightNontunneling(dungeon_t* dungeonPtr, int x, int y);
uint8_t ctype_uint8(int val);

int pathMallocDistGrid(dist_t*** gridPtr) {
    return malloc2DArray((void***) gridPtr, sizeof(dist_t), WIDTH, HEIGHT);
}

void pathFreeDistGrid(dist_t** grid) {
    free2DArray((void**) grid, HEIGHT);
}

int pathTunneling(dungeon_t* dungeonPtr) {
    return pathPopulate(dungeonPtr, dungeonPtr->tunnelingDist, pathCalculateRelWeightTunneling);
}

int pathNontunneling(dungeon_t* dungeonPtr) {
    return pathPopulate(dungeonPtr, dungeonPtr->nontunnelingDist, pathCalculateRelWeightNontunneling);
}

int isBorder(int x, int y) {
    return (x == 0) || (y == 0) || (x == WIDTH - 1) || (y == HEIGHT - 1);
}

int pathPopulate(dungeon_t* dungeonPtr, uint8_t** distGrid, uint8_t (*relDist)(dungeon_t*, int, int)) {
    heap_node_t* binheapNodes[HEIGHT - 2][WIDTH - 2];
    heap_t heap;
    gridNode_t* gridNodePtr;
    int dist;

    heap_init(&heap, pathCmp, NULL);
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == monsterGetX(dungeonPtr->PCPtr) && y == monsterGetY(dungeonPtr->PCPtr)) {
                distGrid[y][x] = 0;
            } else {
                distGrid[y][x] = PATH_DIST_INFINITE;
            }

            if (!isBorder(x, y)) {
                gridNodePtr = malloc(sizeof(gridNode_t));
                gridNodePtr->x = x;
                gridNodePtr->y = y;
                gridNodePtr->weightPtr = &(distGrid[y][x]);
                binheapNodes[y - 1][x - 1] = heap_insert(&heap, gridNodePtr);
            }
        }
    }

    while (heap.size) {
        gridNodePtr = heap_remove_min(&heap);
        for (int offY = -1; offY <= 1; offY++) {
            for (int offX = -1; offX <= 1; offX++) {
                // Ignore border cells
                if (!isBorder(gridNodePtr->x + offX, gridNodePtr->y + offY)) {
                    // Ignore current cell
                    if (!(offX == 0 && offY == 0)) {
                        dist = *gridNodePtr->weightPtr + relDist(dungeonPtr, gridNodePtr->x + offX, gridNodePtr->y + offY);
                        if (dist < distGrid[gridNodePtr->y + offY][gridNodePtr->x + offX]) {
                            distGrid[gridNodePtr->y + offY][gridNodePtr->x + offX] = ctype_uint8(dist);
                            heap_decrease_key_no_replace(&heap, binheapNodes[gridNodePtr->y + offY - 1][gridNodePtr->x + offX - 1]);
                        }
                    }
                }
            }
        }
        free(gridNodePtr);
    }

    heap_delete(&heap);
    return 0;
}

uint8_t ctype_uint8(int val) {
    return val < 255 ? (uint8_t)val : (uint8_t)255;
}

void printDistGrid(dungeon_t* dungeonPtr, uint8_t** grid) {
    char currentChar;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (grid[y][x] + '0' <= '9') {
                currentChar = (char) ('0' + grid[y][x]);
            } else if (grid[y][x] - 10 + 'a' <= 'z') {
                currentChar = (char) ('a' + grid[y][x] - 10);
            } else if (grid[y][x] - 36 + 'A' <= 'Z') {
                currentChar = (char) ('A' + grid[y][x] - 36);
            } else {
                currentChar = dungeonPtr->grid[y][x].material;
            }
            mvaddch(y + 1, x, currentChar);
        }
    }
}

gridNode_t* mallocGridNode(uint8_t* weightPtr, int x, int y) {
    gridNode_t* node = malloc(sizeof(gridNode_t));
    node->weightPtr = weightPtr;
    node->x = x;
    node->y = y;
    return node;
}

int32_t pathCmp(const void *keyPtr, const void *withPtr) {
    const gridNode_t* keyPtr2 = keyPtr;
    const gridNode_t* withPtr2 = withPtr;
    return *keyPtr2->weightPtr - *withPtr2->weightPtr;
}

void pathDataDelete(void* data) {
    free(data);
}

dist_t pathCalculateRelWeightTunneling(dungeon_t* dungeonPtr, int x, int y) {
    return pathCalculateWeight(dungeonPtr->grid[y][x].hardness);
}

dist_t pathCalculateRelWeightNontunneling(dungeon_t* dungeonPtr, int x, int y) {
    if (dungeonPtr->grid[y][x].material != rock) {
        return pathCalculateWeight(dungeonPtr->grid[y][x].hardness);
    }
    return PATH_DIST_INFINITE;
}

dist_t pathCalculateWeight(uint8_t hardness) {
    if (hardness < 85) {
        return 1;
    }
    if (hardness < 170) {
        return 2;
    }
    if (hardness < 255) {
        return 3;
    }
    return 0xFF;
}
