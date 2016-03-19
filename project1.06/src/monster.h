//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_MONSTER_H
#define PROJECT_MONSTER_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dungeon_struct dungeon_t;

#define MONSTER_MIN_SPEED 5
#define MONSTER_MAX_SPEED 20

#define MONSTER_INTELLIGENT 0x8
#define MONSTER_TELEPATHIC 0x4
#define MONSTER_TUNNELING 0x2
#define MONSTER_ERRATIC 0x1

typedef struct monster_struct {
    uint8_t type;
    int speed;
    int isPC;
    int x;
    int y;
    int lastPCX;
    int lastPCY;
    int alive;
} monster_t;

void initMonsters(dungeon_t *dungeonPtr);
void monstersDestroy(dungeon_t *dungeonPtr);
char monsterGetChar(monster_t m);
void monsterGenerate(monster_t *m);
void monsterList(dungeon_t *dungeonPtr);
char *monsterDescription(dungeon_t *dungeonPtr, monster_t *monster);

#ifdef __cplusplus
}
#endif

#endif //PROJECT_MONSTER_H
