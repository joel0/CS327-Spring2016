//
// Created by joelm on 2016-02-07.
//

#ifndef PROJECT_DUNGEON_H
#define PROJECT_DUNGEON_H

#include <inttypes.h>

#include "monster.h"

enum material_enum {
    room = '.',
    corridor = '#',
    rock = ' '
};

typedef enum direction_enum {
    nowhere =   0b0000,
    north =     0b0001,
    northeast = 0b0011,
    east =      0b0010,
    southeast = 0b0110,
    south =     0b0100,
    southwest = 0b1100,
    west =      0b1000,
    northwest = 0b1001
} direction_t;

typedef struct room_struct {
    uint8_t x:8;
    uint8_t y:8;
    uint8_t width:8;
    uint8_t height:8;
} room_t;

typedef struct gridCell_struct {
    uint8_t hardness;
    enum material_enum material;
    monster_t* monsterPtr;
} gridCell_t;

typedef struct dungeon_struct {
    monster_t PC;
    gridCell_t** grid;
    uint8_t** tunnelingDist;
    uint8_t** nontunnelingDist;
    room_t* rooms;
    int roomCount;
    monster_t** monsterPtrs;
    int monsterCount;
} dungeon_t;

int saveDungeon(dungeon_t dungeon, char* fileName);
int loadDungeon(dungeon_t* dungeonPtr, char* fileName);
void destroyDungeon(dungeon_t dungeon);
void dungeonPlaceMonster(dungeon_t* dungeonPtr, monster_t* monsterPtr);
void printRooms(int roomCount, room_t* rooms);
void printMonsters(int monsterCount, monster_t** monsterPtrs);
int generateDungeon(dungeon_t* dungeonPtr);
int roomDist(room_t room1, room_t room2);
void connectRooms(gridCell_t **grid, room_t* rooms, int roomCount);
void connectTwoRooms(gridCell_t **grid, room_t room1, room_t room2);
direction_t calculateDirection(int x, int y, int targetX, int targetY);
int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount);
int validateRoom(room_t* rooms, int roomCount, room_t room);
int validateTwoRooms(room_t room1, room_t room2);
void printDungeon(dungeon_t* dungeonPtr);
int populateGrid(dungeon_t* dungeonPtr);
void populateRooms(dungeon_t dungeon);
void dungeonRemoveMonster(monster_t** monsterPtrs, int toRemove, int* monsterCountPtr);

#endif //PROJECT_DUNGEON_H
