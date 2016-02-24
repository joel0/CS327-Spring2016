//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <stdio.h>

#include "turn.h"
#include "binheap.h"
#include "dungeon.h"
#include "movement.h"
#include "monster.h"

int turnCompare(const void* d1, const void* d2);
void turnDelete(void* d);

void turnInit(dungeon_t* dungeonPtr) {
    turn_t* turn;
    dungeonPtr->turnsHeapPtr = malloc(sizeof(binheap_t));//TODO free that
    binheap_init(dungeonPtr->turnsHeapPtr, turnCompare, turnDelete);
    for (int i = 0; i < dungeonPtr->monsterCount; i++) {
        turn = malloc(sizeof(turn));
        turn->monsterPtr = dungeonPtr->monsterPtrs[i];
        turn->nextTurn = 100 / turn->monsterPtr->speed;
        turn->id = i;
        binheap_insert(dungeonPtr->turnsHeapPtr, (void*) turn);
    }
}

void turnDo(dungeon_t* dungeonPtr) {
    turn_t *turnPtr;
    do {
        turnPtr = binheap_remove_min(dungeonPtr->turnsHeapPtr);
    } while (!turnPtr->monsterPtr->alive);
    moveMonsterLogic(dungeonPtr, *turnPtr->monsterPtr);
    turnPtr->nextTurn += 100 / turnPtr->monsterPtr->speed;
    binheap_insert(dungeonPtr->turnsHeapPtr, (void*) turnPtr);
    printf("Moved %c\n", monsterGetChar(*turnPtr->monsterPtr));
}

void turnDestroy(dungeon_t* dungeonPtr) {
    binheap_delete(dungeonPtr->turnsHeapPtr);
}

int turnCompare(const void* d1, const void* d2) {
    turn_t* t1 = (turn_t*) d1;
    turn_t* t2 = (turn_t*) d2;
    if (t1->nextTurn != t2->nextTurn) {
        return t1->id - t2->id;
    }
    return t1->nextTurn - t2->nextTurn;
}

void turnDelete(void* d) {
    //free(d);
}
