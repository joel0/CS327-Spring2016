//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>

#include "monster.h"
#include "dungeon.h"

// To keep track of how many monsters to free in the destroy
static int totalMonsters;

void initMonsters(dungeon_t* dungeonPtr) {
    monster_t* randMonsterPtr;
    dungeonPtr->PC.isPC = 1;
    dungeonPtr->PC.speed = 10;
    dungeonPtr->PC.alive = 1;
    dungeonPtr->PC.type = MONSTER_TUNNELING;
    dungeonRandomlyPlaceMonster(dungeonPtr, &dungeonPtr->PC);
    dungeonPtr->monsterCount++; // +1 for the PC
    dungeonPtr->monsterPtrs = malloc(sizeof(monster_t*) * dungeonPtr->monsterCount);
    dungeonPtr->monsterPtrs[0] = &dungeonPtr->PC;

    for (int i = 1; i < dungeonPtr->monsterCount; i++) {
        randMonsterPtr = malloc(sizeof(monster_t));
        monsterGenerate(randMonsterPtr);
        dungeonPtr->monsterPtrs[i] = randMonsterPtr;
        dungeonRandomlyPlaceMonster(dungeonPtr, randMonsterPtr);
    }
    totalMonsters = dungeonPtr->monsterCount;
}

void monstersDestroy(dungeon_t* dungeonPtr) {
    for (int i = 1; i < totalMonsters; i++) {
        free(dungeonPtr->monsterPtrs[i]);
    }
    free(dungeonPtr->monsterPtrs);
}

char monsterGetChar(monster_t m) {
    if (m.isPC) {
        return '@';
    }
    if (m.type < 10) {
        return (char) ('0' + m.type);
    }
    return (char) ('a' + m.type - 10);
}

void monsterGenerate(monster_t* m) {
    m->isPC = 0;
    m->speed = MONSTER_MIN_SPEED + (rand() % (MONSTER_MAX_SPEED - MONSTER_MIN_SPEED + 1));
    m->type = (uint8_t) (rand() & 0x0F); // 50% chance of each bit being 1
    m->lastPCX = 0;
    m->lastPCY = 0;
    m->alive = 1;
    m->x = 0;
    m->y = 0;
}