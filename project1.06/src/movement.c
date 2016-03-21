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
void generateDirectMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int targetX, int targetY, int* dstX, int* dstY);
int isLineOfSight(gridCell_t** grid, int x1, int y1, int x2, int y2);

void moveMonsterLogic(dungeon_t* dungeonPtr, monster_t* monsterPtr) {
    if (!monsterIsAlive(monsterPtr)) {
        return;
    }

    int dstX, dstY;
    if (monsterIsPC(monsterPtr)) {
        // PC does not move here.  His logic is handled elsewhere.
        return;
    }
    if (monsterGetType(monsterPtr) & MONSTER_ERRATIC) {
        if (rand() % 2) {
            // random movement
            generateRandMove(dungeonPtr, monsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            return;
        }
    }
    if (monsterGetType(monsterPtr) & MONSTER_INTELLIGENT) {
        // intelligent
        if (monsterGetType(monsterPtr) & MONSTER_TELEPATHIC) {
            generateShortestMove(dungeonPtr, monsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
        } else {
            // Only move if the PC is visible or there is a last known location of the PC, otherwise stay put.
            if (isLineOfSight(dungeonPtr->grid, monsterGetX(monsterPtr), monsterGetY(monsterPtr), monsterGetX(dungeonPtr->PCPtr), monsterGetY(dungeonPtr->PCPtr))) {
                // PC is visible
                generateShortestMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            } else if (monsterGetLastPCX(monsterPtr)) {
                // PC is remembered
                generateDirectMove(dungeonPtr, monsterPtr, monsterGetLastPCX(monsterPtr), monsterGetLastPCY(monsterPtr), &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            }
        }
    } else {
        // not intelligent
        if (monsterGetType(monsterPtr) & MONSTER_TELEPATHIC) {
            generateDirectMove(dungeonPtr, monsterPtr, monsterGetX(dungeonPtr->PCPtr), monsterGetY(dungeonPtr->PCPtr), &dstX, &dstY);
            moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
        } else {
            // not telepathic
            if (isLineOfSight(dungeonPtr->grid, monsterGetX(monsterPtr), monsterGetY(monsterPtr), monsterGetX(dungeonPtr->PCPtr), monsterGetY(dungeonPtr->PCPtr))){
                // player is visible
                generateDirectMove(dungeonPtr, monsterPtr, monsterGetX(dungeonPtr->PCPtr), monsterGetY(dungeonPtr->PCPtr), &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            } else {
                // player is not visible, move randomly
                generateRandMove(dungeonPtr, monsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, monsterPtr, dstX, dstY);
            }
        }
    }

    // Keep track of line-of-sight memory
    if (isLineOfSight(dungeonPtr->grid, monsterGetX(monsterPtr), monsterGetY(monsterPtr), monsterGetX(dungeonPtr->PCPtr), monsterGetY(dungeonPtr->PCPtr))) {
        monsterSetLastPCX(monsterPtr, monsterGetX(dungeonPtr->PCPtr));
        monsterSetLastPCY(monsterPtr, monsterGetY(dungeonPtr->PCPtr));
    }
}

void moveMonster(dungeon_t* dungeonPtr, monster_t* monsterPtr, int dstX, int dstY) {
    int srcX = monsterGetX(monsterPtr);
    int srcY = monsterGetY(monsterPtr);
    gridCell_t** grid = dungeonPtr->grid;

    // Do not do anything if the "move" is nothing
    if (monsterGetX(monsterPtr) == dstX && monsterGetY(monsterPtr) == dstY) {
        return;
    }
    // Do nothing if the monster (or PC) is trying to go into immutable rock
    if (grid[dstY][dstX].hardness == ROCK_HARDNESS_IMMUTABLE) {
        return;
    }

    if (grid[dstY][dstX].monsterPtr != NULL) {
        // A monster is eating another.  We must delete the eaten one.
        int toRemove = -1;
        while (dungeonPtr->monsterPtrs[++toRemove] != grid[dstY][dstX].monsterPtr)
            ;
        dungeonRemoveMonster(dungeonPtr->monsterPtrs, toRemove, &dungeonPtr->monsterCount);
    }
    if (grid[dstY][dstX].hardness > 85) {
        // The rock must be bored through
        grid[dstY][dstX].hardness -= 85;
        pathTunneling(dungeonPtr);
    } else {
        grid[dstY][dstX].monsterPtr = grid[srcY][srcX].monsterPtr;
        grid[srcY][srcX].monsterPtr = NULL;
        monsterSetX(grid[dstY][dstX].monsterPtr, dstX);
        monsterSetY(grid[dstY][dstX].monsterPtr, dstY);

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
    int tunneling = monsterGetType(monsterPtr) & MONSTER_TUNNELING;
    int targetX, targetY;
    do {
        movementDir = utilRandDir();
        targetX = monsterGetX(monsterPtr);
        targetY = monsterGetY(monsterPtr);
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
    int x = monsterGetX(monsterPtr);
    int y = monsterGetY(monsterPtr);
    uint8_t** grid;
    uint8_t shortestDist;
    direction_t shortestDirection;

    if (monsterGetType(monsterPtr) & MONSTER_TUNNELING) {
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

void generateDirectMove(dungeon_t* dungeonPtr, monster_t* monsterPtr, int PCX, int PCY, int* dstX, int* dstY) {
    int targetX = monsterGetX(monsterPtr);
    int targetY = monsterGetY(monsterPtr);
    direction_t direction = calculateDirection(targetX, targetY, PCX, PCY);
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
    if (!(monsterGetType(monsterPtr) & MONSTER_TUNNELING)) {
        // Nontuneling
        if (isRock(dungeonPtr->grid, targetX, targetY)) {
            // Direct path is blocked, try orthogonal
            targetX = monsterGetX(monsterPtr);
            targetY = monsterGetY(monsterPtr);
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
    int topX, topY;
    int bottomY;
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
    if (y1 < y2) {
        topX = x1;
        topY = y1;
        bottomY = y2;
    } else {
        topX = x2;
        topY = y2;
        bottomY = y1;
    }
    slope = ((double) (leftY - rightY)) / (leftX - rightX);
    for (int x = leftX; x < rightX; x++) {
        int y = (int) (round((x - leftX) * slope) + leftY);
        if (grid[y][x].hardness != 0) {
            return 0;
        }
    }
    for (int y = topY; y < bottomY; y++) {
        int x = (int) (round((y - topY) / slope) + topX);
        if (grid[y][x].hardness != 0) {
            return 0;
        }
    }
    return 1;
}