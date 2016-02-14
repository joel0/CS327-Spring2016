//
// Created by joelm on 2016-02-07.
//

#include <stdlib.h>
#include <stdio.h>
#include "path.h"
#include "dungeon.h"
#include "globals.h"
#include "binheap.h"
#include "utils.h"

typedef struct gridNode_struct {
    int x;
    int y;
    uint8_t* weightPtr;
} gridNode_t;

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
    //malloc2DArray((void ***) &dungeonPtr->tunnelingDist, sizeof(dist_t), WIDTH, HEIGHT);
    return pathPopulate(dungeonPtr, dungeonPtr->tunnelingDist, pathCalculateRelWeightTunneling);
}

int pathNontunneling(dungeon_t* dungeonPtr) {
    //malloc2DArray((void ***) &dungeonPtr->nontunnelingDist, sizeof(dist_t), WIDTH, HEIGHT);
    return pathPopulate(dungeonPtr, dungeonPtr->nontunnelingDist, pathCalculateRelWeightNontunneling);
}

int pathPopulate(dungeon_t* dungeonPtr, uint8_t** distGrid, uint8_t (*relDist)(dungeon_t*, int, int)) {
    binheap_t heap;
    binheap_node_t*** binheapNodes;

    malloc2DArray((void ***) &binheapNodes, sizeof(binheap_node_t*), WIDTH, HEIGHT);
    binheap_init(&heap, pathCmp, pathDataDelete);

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == dungeonPtr->PCX && y == dungeonPtr->PCY) {
                distGrid[y][x] = 0;
            } else {
                distGrid[y][x] = PATH_DIST_INFINITE;
            }
            // Insert anything that's not an edge.
            if (y != 0 && y != HEIGHT - 1 && x != 0 && x != WIDTH - 1) {
                binheapNodes[y][x] = binheap_insert(&heap, mallocGridNode(&distGrid[y][x], x, y));
            }
        }
    }

    gridNode_t* curNodePtr;
    int neighborCalculatedWeight;
    uint8_t neighborCurWeight;
    while (!binheap_is_empty(&heap)) {
        curNodePtr = binheap_remove_min(&heap);
        for (int y = curNodePtr->y - 1; y < curNodePtr->y + 2; y++) {
            for (int x = curNodePtr->x - 1; x < curNodePtr->x + 2; x++) {
                // Avoid current cell and any edge cells
                if ((x != curNodePtr->x || y != curNodePtr->y) &&
                    x > 0 && x < WIDTH - 1 && y > 0 && y < HEIGHT - 1) {
                    neighborCurWeight = distGrid[y][x];
                    neighborCalculatedWeight = (int)*curNodePtr->weightPtr + relDist(dungeonPtr, x, y); //pathCalculateWeight(dungeonPtr->grid[y][x].hardness);
                    if (neighborCalculatedWeight < neighborCurWeight) {
                        distGrid[y][x] = ctype_uint8(neighborCalculatedWeight);
                        binheap_decrease_key(&heap, binheapNodes[y][x]);
                    }
                }
            }
        }
        free(curNodePtr);
    }
    binheap_delete(&heap);
    return 0;
}

uint8_t ctype_uint8(int val) {
    return val < 255 ? (uint8_t)val : (uint8_t)255;
}

void printDistGrid(uint8_t** grid) {
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
                currentChar = ' ';
            }
            printf("%c", currentChar);
        }
        printf("\n");
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
    if (dungeonPtr->grid[y][x].material == corridor || dungeonPtr->grid[y][x].material == room) {
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