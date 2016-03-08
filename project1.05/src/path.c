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
    binheap_t queue;
    binheap_init(&queue, pathCmp, NULL);

    gridNode_t** objects = malloc(sizeof(gridNode_t) * HEIGHT * WIDTH);
    binheap_node_t*** bh_ptr = malloc(sizeof(binheap_node_t*) * HEIGHT * WIDTH);

    // No need to add character: he starts in a room
    for (int y = 0; y < HEIGHT; y++) {
        objects[y] = malloc(sizeof(gridNode_t) * WIDTH);
        bh_ptr[y] = malloc(sizeof(binheap_node_t*) * WIDTH);
        for (int x = 0; x < HEIGHT; x++) {
            objects[y][x].x = x;
            objects[y][x].y = y;
            if (y != dungeonPtr->PC.y || x != dungeonPtr->PC.x) {
                distGrid[y][x] = 255;
            } else {
                distGrid[y][x] = 0;
            }
            objects[y][x].weightPtr = &distGrid[y][x];
            bh_ptr[y][x] = binheap_insert(&queue, &objects[y][x]);
        }
    }

    gridNode_t* c;

    while ((c = (gridNode_t*) binheap_remove_min(&queue))) {
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int x = c->x + dx;
                int y = c->y + dy;
                int dist = 1 + *objects[c->y][c->x].weightPtr;

                if ((dx != dy) && x > 0 && y > 0 && x < WIDTH && y < HEIGHT) {
                    if (!isBorder(x, y) && (dist < *objects[y][x].weightPtr || *objects[y][x].weightPtr < 0)) {
                        *objects[y][x].weightPtr = dist;
                        binheap_decrease_key(&queue, bh_ptr[y][x]);
                    }
                }
            }
        }
    }

    // No need to add character: he starts in a room
//    for (int y = 0; y < HEIGHT; y++) {
//        for (int x = 0; x < WIDTH; x++) {
//            if (current->rooms_layer[y][x] != ' ' || current->hallways_layer[y][x] != ' ') {
//                current->main_character.player_distances[y][x] = objects[y][x].distance;
//            }
//        }
//    }

    for (int y = 0; y < HEIGHT; y++) {
        free(objects[y]);
        free(bh_ptr[y]);
    }
    free(objects);
    free(bh_ptr);
    binheap_delete(&queue);
}

int isBorder(int x, int y) {
    return (x == 0) || (y == 0) || (x == WIDTH - 1) || (y = HEIGHT - 1);
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
            //printf("%c", currentChar);
        }
        //printf("\n");
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
    //return pathCalculateWeight(dungeonPtr->grid[y][x].hardness);
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
