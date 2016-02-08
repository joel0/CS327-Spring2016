//
// Created by joelm on 2016-02-07.
//

#ifndef PROJECT_DUNGEON_H
#define PROJECT_DUNGEON_H

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
} gridCell_t;

int saveDungeon(gridCell_t** grid, int roomCount, room_t* rooms, char* fileName);
int loadDungeon(gridCell_t*** gridPtr, int* roomCount, room_t** roomsPtr, char* fileName);
void destroyDungeon(gridCell_t** gridPtr);
void showUsage(char* name);
char* dungeonFileName();
void printRooms(int roomCount, room_t* rooms);
int generateDungeon(gridCell_t*** gridPtr, room_t** roomsPtr);
int roomDist(room_t room1, room_t room2);
void connectRooms(gridCell_t **grid, room_t* rooms, int roomCount);
void connectTwoRooms(gridCell_t **grid, room_t room1, room_t room2);
direction_t calculateDirection(int x, int y, int targetX, int targetY);
int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount);
int validateRoom(room_t* rooms, int roomCount, room_t room);
int validateTwoRooms(room_t room1, room_t room2);
void printGrid(gridCell_t** grid);
gridCell_t** populateGrid(room_t* rooms, int roomCount);
void populateRooms(gridCell_t** grid, room_t* rooms, int roomCount);
int malloc2DGrid(gridCell_t*** grid, int width, int height);
void free2DGrid(gridCell_t** grid, int height);

#endif //PROJECT_DUNGEON_H
