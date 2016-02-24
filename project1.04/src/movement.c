//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "movement.h"
#include "utils.h"
#include "globals.h"
#include "path.h"

int isRock(gridCell_t** grid, int x, int y);
int isImmutable(gridCell_t **grid, int x, int y);
void generateRandMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY);
void generateShortestMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY);
void generateDirectMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY);
void moveMonster(dungeon_t* dungeonPtr, monster_t* monsterPtr, int dstX, int dstY);
int isLineOfSight(gridCell_t** grid, int x1, int y1, int x2, int y2);

//void movePC(dungeon_t* dungeonPtr) {
//    int targetX, targetY;
//    generateRandMove(dungeonPtr, dungeonPtr->PC.x, dungeonPtr->PC.y, &targetX, &targetY, 1);
//    moveMonster(dungeonPtr, dungeonPtr->PC.x, dungeonPtr->PC.y, targetX, targetY);
//}

void moveMonsterLogic(dungeon_t* dungeonPtr, monster_t* monsterPtr) {
    if (!monsterPtr->alive) {
        return;
    }

    int dstX, dstY;
    if (monsterPtr->isPC) {
        // PC gets pure random movement
        generateRandMove(dungeonPtr, monsterPtr, &dstX, &dstY);
        moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
        pathTunneling(dungeonPtr);
        pathNontunneling(dungeonPtr);
    }
    if (monsterPtr->type & MONSTER_ERRATIC) {
        if (rand() % 2) {
            // random movement
            generateRandMove(dungeonPtr, monsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            return;
        }
    }
    if (monsterPtr->type & MONSTER_INTELLIGENT) {
        // intelligent
        if (monsterPtr->type & MONSTER_TELEPATHIC) {
            generateShortestMove(dungeonPtr, monsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
        } else {
            // Only move if the PC is visible or there is a last known location of the PC, otherwise stay put.
            if (isLineOfSight(dungeonPtr->grid, monsterPtr->x, monsterPtr->y, dungeonPtr->PC.x, dungeonPtr->PC.y)) {
                // PC is visible
                generateShortestMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            } else if (monsterPtr->lastPCX) {
                // PC is remembered
                generateDirectMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            }
        }
    } else {
        // not intelligent
        if (monsterPtr->type & MONSTER_TELEPATHIC) {
            generateDirectMove(dungeonPtr, monsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
        } else {
            // not telepathic
            if (isLineOfSight(dungeonPtr->grid, monsterPtr->x, monsterPtr->y, dungeonPtr->PC.x, dungeonPtr->PC.y)){
                // player is visible
                generateDirectMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            } else {
                // player is not visible, move randomly
                generateRandMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            }
        }
    }

    // Keep track of line-of-sight memory
    if (isLineOfSight(dungeonPtr->grid, monsterPtr->x, monsterPtr->y, dungeonPtr->PC.x, dungeonPtr->PC.y)) {
        monsterPtr->lastPCX = dungeonPtr->PC.x;
        monsterPtr->lastPCY = dungeonPtr->PC.y;
    }
}

void moveMonster(dungeon_t* dungeonPtr, monster_t* monsterPtr, int dstX, int dstY) {
    int srcX = monsterPtr->x;
    int srcY = monsterPtr->y;
    gridCell_t** grid = dungeonPtr->grid;

    // Do not do anything if the "move" is nothing
    if (monsterPtr->x == dstX && monsterPtr->y == dstY) {
        return;
    }

    if (grid[dstY][dstX].monsterPtr != NULL) {
        // A monster is eating another.  We must delete the eaten one.
        int toRemove = -1;
        while (dungeonPtr->monsterPtrs[++toRemove] != grid[dstY][dstX].monsterPtr)
            ;
        dungeonRemoveMonster(dungeonPtr->monsterPtrs, toRemove, &dungeonPtr->monsterCount);
    }
    if (grid[dstY][dstX].hardness > 85 && !monsterPtr->isPC) {
        // The rock must be bored through
        grid[dstY][dstX].hardness -= 85;
        pathTunneling(dungeonPtr);
    } else {
        grid[dstY][dstX].monsterPtr = grid[srcY][srcX].monsterPtr;
        grid[srcY][srcX].monsterPtr = NULL;
        grid[dstY][dstX].monsterPtr->x = dstX;
        grid[dstY][dstX].monsterPtr->y = dstY;

        // tunneling
        if (grid[dstY][dstX].material == rock) {
            grid[dstY][dstX].material = corridor;
            grid[dstY][dstX].hardness = 0;
            pathTunneling(dungeonPtr);
            pathNontunneling(dungeonPtr);
        }
    }
}

void generateRandMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY) {
    direction_t movementDir;
    int tunneling = monsterPtr->type & MONSTER_TUNNELING;
    int targetX, targetY;
    do {
        movementDir = utilRandDir();
        targetX = monsterPtr->x;
        targetY = monsterPtr->y;
        if (movementDir & north) {
            targetY--;
        } else if (movementDir & south) {
            targetY++;
        }
        if (movementDir & west) {
            targetX--;
        } else if (movementDir & east) {
            targetX++;
        }
    } while (tunneling ? isImmutable(dungeonPtr->grid, targetX, targetY) : isRock(dungeonPtr->grid, targetX, targetY));
    *dstX = targetX;
    *dstY = targetY;
}

void generateShortestMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY) {
    int x = monsterPtr->x;
    int y = monsterPtr->y;
    uint8_t** grid;
    uint8_t shortestDist;
    direction_t shortestDirection;

    if (monsterPtr->type & MONSTER_TUNNELING) {
        grid = dungeonPtr->tunnelingDist;
    } else {
        grid = dungeonPtr->nontunnelingDist;
    };
    shortestDist = grid[y - 1][x - 1];
    shortestDirection = northwest;

    if (grid[y - 1][x] < shortestDist) {
        shortestDist = grid[y - 1][x];
        shortestDirection = north;
    }
    if (grid[y - 1][x + 1] < shortestDist) {
        shortestDist = grid[y - 1][x + 1];
        shortestDirection = northeast;
    }
    if (grid[y][x - 1] < shortestDist) {
        shortestDist = grid[y][x - 1];
        shortestDirection = west;
    }
    if (grid[y][x + 1] < shortestDist) {
        shortestDist = grid[y][x + 1];
        shortestDirection = east;
    }
    if (grid[y + 1][x - 1] < shortestDist) {
        shortestDist = grid[y + 1][x - 1];
        shortestDirection = southwest;
    }
    if (grid[y + 1][x] < shortestDist) {
        shortestDist = grid[y + 1][x];
        shortestDirection = south;
    }
    if (grid[y + 1][x + 1] < shortestDist) {
        shortestDirection = southeast;
    }
    if (shortestDirection & north) {
        y--;
    }
    if (shortestDirection & east) {
        x++;
    }
    if (shortestDirection & south) {
        y++;
    }
    if (shortestDirection & west) {
        x--;
    }
    *dstX = x;
    *dstY = y;
}

void generateDirectMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int* dstX, int* dstY) {
    int targetX = monsterPtr->x;
    int targetY = monsterPtr->y;
    direction_t direction = calculateDirection(targetX, targetY, dungeonPtr->PC.x, dungeonPtr->PC.y);
    if (direction & north) {
        targetY--;
    }
    if (direction & east) {
        targetX++;
    }
    if (direction & south) {
        targetY++;
    }
    if (direction & west) {
        targetX--;
    }
    if (!(monsterPtr->type & MONSTER_TUNNELING)) {
        // Nontuneling
        if (isRock(dungeonPtr->grid, targetX, targetY)) {
            // Direct path is blocked, try orthogonal
            targetX = monsterPtr->x;
            targetY = monsterPtr->y;
            if (direction & north) {
                if (!isRock(dungeonPtr->grid, targetX, targetY - 1)) {
                    targetY--;
                }
            }
            if (direction & east) {
                if (!isRock(dungeonPtr->grid, targetX + 1, targetY)) {
                    targetX++;
                }
            }
            if (direction & south) {
                if (!isRock(dungeonPtr->grid, targetX, targetY + 1)) {
                    targetY++;
                }
            }
            if (direction & west) {
                if (!isRock(dungeonPtr->grid, targetX - 1, targetY)) {
                    targetX--;
                }
            }
        }
    }
    *dstX = targetX;
    *dstY = targetY;
}

int isRock(gridCell_t** grid, int x, int y) {
    return grid[y][x].material == rock;
}

int isImmutable(gridCell_t **grid, int x, int y) {
    return grid[y][x].hardness == ROCK_HARDNESS_IMMUTABLE;
}

int isLineOfSight(gridCell_t** grid, int x1, int y1, int x2, int y2) {
    double slope;
    int leftX, leftY;
    int rightX, rightY;
    if (x1 < x2) {
        leftX = x1;
        leftY = y1;
        rightX = x2;
        rightY = y2;
    } else {
        leftX = x2;
        leftY = y2;
        rightX = x1;
        rightY = y1;
    }
    slope = ((double) (leftY - rightY)) / (leftX - rightX);
    for (int x = leftX; x < rightX; x++) {
        int y = (int) (round((x - leftX) * slope) + leftY);
        if (grid[y][x].hardness != 0) {
            return 0;
        }
    }
    return 1;
}