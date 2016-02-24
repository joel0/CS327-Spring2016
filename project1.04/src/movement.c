//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>

#include "movement.h"
#include "utils.h"
#include "globals.h"

int isRock(gridCell_t** grid, int x, int y);
int isImmutable(gridCell_t **grid, int x, int y);
void generateRandMove(dungeon_t* dungeonPtr, int srcX, int srcY, int* dstX, int* dstY, int tunneling);
void moveMonster(dungeon_t* dungeonPtr, int srcX, int srcY, int dstX, int dstY);

void movePC(dungeon_t* dungeonPtr) {
    int targetX, targetY;
    generateRandMove(dungeonPtr, dungeonPtr->PC.x, dungeonPtr->PC.y, &targetX, &targetY, 1);
    moveMonster(dungeonPtr, dungeonPtr->PC.x, dungeonPtr->PC.y, targetX, targetY);
}

void moveMonsterLogic(dungeon_t* dungeonPtr, monster_t m) {
    if (!m.alive) {
        return;
    }

    int dstX, dstY;
    if (m.type & MONSTER_ERRATIC) {
        if (rand() % 2) {
            // random movement
            generateRandMove(dungeonPtr, m.x, m.y, &dstX, &dstY, m.type & MONSTER_TUNNELING);
            return;
        }
    }
    //TODO
    generateRandMove(dungeonPtr, m.x, m.y, &dstX, &dstY, m.type & MONSTER_TUNNELING);
    moveMonster(dungeonPtr, m.x, m.y, dstX, dstY);
}

void moveMonster(dungeon_t* dungeonPtr, int srcX, int srcY, int dstX, int dstY) {
    gridCell_t** grid = dungeonPtr->grid;
    if (grid[dstY][dstX].monsterPtr != NULL) {
        // A monster is eating another.  We must delete the eaten one.
        int toRemove = 0; // skipping the PC at index 0
        while (dungeonPtr->monsterPtrs[++toRemove] != grid[dstY][dstX].monsterPtr)
            ;
        dungeonRemoveMonster(dungeonPtr->monsterPtrs, toRemove, &dungeonPtr->monsterCount);
    }
    grid[dstY][dstX].monsterPtr = grid[srcY][srcX].monsterPtr;
    grid[srcY][srcX].monsterPtr = NULL;
    grid[dstY][dstX].monsterPtr->x = dstX;
    grid[dstY][dstX].monsterPtr->y = dstY;

    // tunneling
    if (grid[dstY][dstX].material == rock) {
        grid[dstY][dstX].material = corridor;
        grid[dstY][dstX].hardness = 0;
    }
}

void generateRandMove(dungeon_t* dungeonPtr, int srcX, int srcY, int* dstX, int* dstY, int tunneling) {
    direction_t movementDir;
    int targetX, targetY;
    do {
        movementDir = utilRandDir();
        targetX = srcX;
        targetY = srcY;
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

int isRock(gridCell_t** grid, int x, int y) {
    return grid[y][x].material == rock;
}

int isImmutable(gridCell_t **grid, int x, int y) {
    return grid[y][x].hardness == ROCK_HARDNESS_IMMUTABLE;
}

//int isLineOfSight