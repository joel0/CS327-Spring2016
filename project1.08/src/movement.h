//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_MOVEMENT_H
#define PROJECT_MOVEMENT_H

#include "dungeon.h"

void moveMonsterLogic(dungeon_t *dungeonPtr, monster *monsterPtr);
void moveMonster(dungeon_t *dungeonPtr, monster *monsterPtr, int dstX, int dstY);
int movementIsLineOfSight(gridCell_t **grid, int x1, int y1, int x2, int y2);

#endif //PROJECT_MOVEMENT_H
