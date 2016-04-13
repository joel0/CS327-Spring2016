//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <sstream>

#include "movement.h"
#include "utils.h"
#include "path.h"
#include "message_queue.h"

int isRock(gridCell_t** grid, int x, int y);
int isImmutable(gridCell_t **grid, int x, int y);
void generateRandMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int* dstX, int* dstY);
void generateShortestMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int* dstX, int* dstY);
void generateDirectMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int targetX, int targetY, int* dstX, int* dstY);

void moveMonsterLogic(dungeon_t* dungeonPtr, monster* monsterPtr) {
    monster_evil* evilMonsterPtr;
    if (!monsterPtr->alive) {
        return;
    }

    int dstX, dstY;
    if (monsterPtr->isPC()) {
        // PC does not move here.  His logic is handled elsewhere.
        return;
    }
    evilMonsterPtr = (monster_evil*) monsterPtr;
    if (evilMonsterPtr->abilities & MONSTER_ERRATIC) {
        if (rand() % 2) {
            // random movement
            generateRandMove(dungeonPtr, evilMonsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
            return;
        }
    }
    if (evilMonsterPtr->abilities & MONSTER_INTELLIGENT) {
        // intelligent
        if (evilMonsterPtr->abilities & MONSTER_TELEPATHIC) {
            generateShortestMove(dungeonPtr, evilMonsterPtr, &dstX, &dstY);
            moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
        } else {
            // Only move if the PC is visible or there is a last known location of the PC, otherwise stay put.
            if (movementIsLineOfSight(dungeonPtr->grid, evilMonsterPtr->x, evilMonsterPtr->y,
                                      dungeonPtr->PCPtr->x, dungeonPtr->PCPtr->y)) {
                // PC is visible
                generateShortestMove(dungeonPtr, evilMonsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
            } else if (evilMonsterPtr->lastPCX) {
                // PC is remembered
                generateDirectMove(dungeonPtr, evilMonsterPtr, evilMonsterPtr->lastPCX, evilMonsterPtr->lastPCY, &dstX, &dstY);
                moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
            }
        }
    } else {
        // not intelligent
        if (evilMonsterPtr->abilities & MONSTER_TELEPATHIC) {
            generateDirectMove(dungeonPtr, evilMonsterPtr, dungeonPtr->PCPtr->x, dungeonPtr->PCPtr->y, &dstX, &dstY);
            moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
        } else {
            // not telepathic
            if (movementIsLineOfSight(dungeonPtr->grid, evilMonsterPtr->x, evilMonsterPtr->y,
                                      dungeonPtr->PCPtr->x, dungeonPtr->PCPtr->y)){
                // player is visible
                generateDirectMove(dungeonPtr, evilMonsterPtr, dungeonPtr->PCPtr->x, dungeonPtr->PCPtr->y, &dstX, &dstY);
                moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
            } else {
                // player is not visible, move randomly
                generateRandMove(dungeonPtr, evilMonsterPtr, &dstX, &dstY);
                moveMonster(dungeonPtr, evilMonsterPtr, dstX, dstY);
            }
        }
    }

    // Keep track of line-of-sight memory
    if (movementIsLineOfSight(dungeonPtr->grid, evilMonsterPtr->x, evilMonsterPtr->y,
                              dungeonPtr->PCPtr->x, dungeonPtr->PCPtr->y)) {
        evilMonsterPtr->lastPCX = dungeonPtr->PCPtr->x;
        evilMonsterPtr->lastPCY = dungeonPtr->PCPtr->y;
    }
}

void moveMonster(dungeon_t* dungeonPtr, monster* monsterPtr, int dstX, int dstY) {
    int srcX = monsterPtr->x;
    int srcY = monsterPtr->y;
    gridCell_t** grid = dungeonPtr->grid;
    monster* replacement_ptr = NULL;

    // Do not do anything if the "move" is nothing
    if (monsterPtr->x == dstX && monsterPtr->y == dstY) {
        return;
    }
    // Do nothing if the monster (or PC) is trying to go into immutable rock
    if (grid[dstY][dstX].hardness == ROCK_HARDNESS_IMMUTABLE) {
        return;
    }

    if (grid[dstY][dstX].monsterPtr != NULL) {
        // A monster is eating another.
        if (!monsterPtr->isPC() && !grid[dstY][dstX].monsterPtr->isPC()) {
            // This combat does not involve the PC.  We must move the monster out of the way.
            // TODO shove the monster instead of switching positions
            replacement_ptr = grid[dstY][dstX].monsterPtr;
        } else {
            // Combat with the PC
            std::stringstream msg_str;

            int dam = monsterPtr->attack(*grid[dstY][dstX].monsterPtr);
            if (grid[dstY][dstX].monsterPtr->alive) {
                // The monster in the target location did not die.  No movement happens.
                msg_str << monsterPtr->name << " attacked " << grid[dstY][dstX].monsterPtr->name << " for " <<
                dam << ". HP: " << grid[dstY][dstX].monsterPtr->HP;
                message_queue::instance()->enqueue(msg_str);
                return;
            }

            msg_str << monsterPtr->name << " attacked and killed " << grid[dstY][dstX].monsterPtr->name <<
            " with " << dam << " damage";

            int toRemove = -1;
            while (dungeonPtr->monsterPtrs[++toRemove] != grid[dstY][dstX].monsterPtr);
            dungeonRemoveMonster(dungeonPtr->monsterPtrs, toRemove, &dungeonPtr->monsterCount);
        }
    }
    if (grid[dstY][dstX].hardness > 85) {
        // The rock must be bored through
        grid[dstY][dstX].hardness -= 85;
        pathTunneling(dungeonPtr);
    } else {
        grid[dstY][dstX].monsterPtr = grid[srcY][srcX].monsterPtr;
        grid[srcY][srcX].monsterPtr = replacement_ptr;
        grid[dstY][dstX].monsterPtr->x = dstX;
        grid[dstY][dstX].monsterPtr->y = dstY;
        if (replacement_ptr != NULL) {
            replacement_ptr->x = srcX;
            replacement_ptr->y = srcY;
        }

        // tunneling
        if (grid[dstY][dstX].material == rock) {
            grid[dstY][dstX].material = corridor;
            grid[dstY][dstX].hardness = 0;
            pathTunneling(dungeonPtr);
            pathNontunneling(dungeonPtr);
        }
    }
}

void generateRandMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int* dstX, int* dstY) {
    direction_t movementDir;
    int tunneling = monsterPtr->abilities & MONSTER_TUNNELING;
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

void generateShortestMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int* dstX, int* dstY) {
    int x = monsterPtr->x;
    int y = monsterPtr->y;
    uint8_t** grid;
    uint8_t shortestDist;
    direction_t shortestDirection;

    if (monsterPtr->abilities & MONSTER_TUNNELING) {
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

void generateDirectMove(dungeon_t* dungeonPtr, monster_evil* monsterPtr, int PCX, int PCY, int* dstX, int* dstY) {
    int targetX = monsterPtr->x;
    int targetY = monsterPtr->y;
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
    if (!(monsterPtr->abilities & MONSTER_TUNNELING)) {
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

int movementIsLineOfSight(gridCell_t **grid, int x1, int y1, int x2, int y2) {
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