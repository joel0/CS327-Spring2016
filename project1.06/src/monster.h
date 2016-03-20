//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_MONSTER_H
#define PROJECT_MONSTER_H

#include <inttypes.h>

#define MONSTER_MIN_SPEED 5
#define MONSTER_MAX_SPEED 20

#define MONSTER_INTELLIGENT 0x8
#define MONSTER_TELEPATHIC 0x4
#define MONSTER_TUNNELING 0x2
#define MONSTER_ERRATIC 0x1

typedef struct dungeon_struct dungeon_t;

#ifdef __cplusplus
class monster {
    // To keep track of how many monsters to free in the destroy
    static int totalMonsters;
public:
    uint8_t type;
    int speed;
    int x;
    int y;
    int lastPCX;
    int lastPCY;
    bool alive;

protected:
    monster() { }

public:
    monster(uint8_t type, int speed, int x, int y) {
        this->type = type;
        this->speed = speed;
        this->x = x;
        this->y = y;
        this->lastPCX = x;
        this->lastPCY = y;
        this->alive = true;
    }
public:
    char* toString(dungeon_t* dungeonPtr);
    // Override getChar
    inline virtual char getChar() { return '!'; }
    inline virtual bool isPC() { return false; }
};

class monster_evil : public monster {
public:
    monster_evil();
    monster_evil(uint8_t type, int speed, int x, int y) : monster(type, speed, x, y) {}
    char getChar();
};

class monster_PC : public monster {
public:
    inline char getChar() { return '@'; }
    monster_PC(int x, int y) : monster(MONSTER_TUNNELING, 10, x, y) {}

public:
    inline bool isPC() { return true; }
};

extern "C" {
#endif

typedef struct dungeon_struct dungeon_t;

//typedef struct monster_struct {
//    uint8_t type;
//    int speed;
//    int isPC;
//    int x;
//    int y;
//    int lastPCX;
//    int lastPCY;
//    int alive;
//} monster_t;
typedef void* monster_t;

void initMonsters(dungeon_t *dungeonPtr);
void monstersDestroy(dungeon_t *dungeonPtr);
char monsterGetChar(monster_t* m);
void monsterGenerate(monster_t *m);
void monsterList(dungeon_t *dungeonPtr);
char *monsterDescription(dungeon_t *dungeonPtr, monster_t *monsterPtr);
int monsterGetX(monster_t* monsterRef);
int monsterGetY(monster_t* monsterRef);
int monsterGetLastPCX(monster_t* monsterPtr);
int monsterGetLastPCY(monster_t* monsterPtr);
void monsterSetX(monster_t* monsterPtr, int x);
void monsterSetY(monster_t* monsterPtr, int x);
void monsterSetLastPCX(monster_t* monsterPtr, int x);
void monsterSetLastPCY(monster_t* monsterPtr, int x);
int monsterIsAlive(monster_t* monsterRef);
int monsterIsPC(monster_t* monsterRef);
int monsterSpeed(monster_t* monsterPtr);
void monsterKill(monster_t* monsterPtr);
uint8_t monsterGetType(monster_t* monsterPtr);

#ifdef __cplusplus
}
#endif

#endif //PROJECT_MONSTER_H
