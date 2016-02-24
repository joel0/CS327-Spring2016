//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>

#include "monster.h"

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
    m->type = (uint8_t) (rand() % 0x10); // 50% chance of each bit being 1
    //m->type = 0b1100;
}