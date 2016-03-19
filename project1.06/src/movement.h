//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_MOVEMENT_H
#define PROJECT_MOVEMENT_H

#include "dungeon.h"

void moveMonsterLogic(dungeon_t* dungeonPtr, monster_t* monsterPtr);
void moveMonster(dungeon_t* dungeonPtr, monster_t* monsterPtr, int dstX, int dstY);
//void movePC(dungeon_t* dungeonPtr);

#endif //PROJECT_MOVEMENT_H
