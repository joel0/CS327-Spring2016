//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_TURN_H
#define PROJECT_TURN_H

#include "monster.h"
#include "dungeon.h"

typedef struct turn_struct {
    int id;
    int nextTurn;
    monster_t* monsterPtr;
} turn_t;

void turnInit(dungeon_t* dungeonPtr);
void turnDo(dungeon_t* dungeonPtr);
int turnIsPC(dungeon_t* dungeonPtr);
void turnDestroy(dungeon_t* dungeonPtr);

#endif //PROJECT_TURN_H
