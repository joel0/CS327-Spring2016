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
    monster* monsterPtr;
} turn_t;

typedef enum PC_action_enum {
    actionStairsDn,
    actionStairsUp,
    actionListMonsters,
    actionListInventory,
    actionListEquipment,
    actionDropItem,
    actionInspectItem,
    actionSave,
    actionMovement
} PC_action;

void turnInit(dungeon_t* dungeonPtr);
void turnDo(dungeon_t* dungeonPtr);
PC_action turnDoPC(dungeon_t* dungeonPtr);
int turnIsPC(dungeon_t* dungeonPtr);
void turnDestroy(dungeon_t* dungeonPtr);

#endif //PROJECT_TURN_H
