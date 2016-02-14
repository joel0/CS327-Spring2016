//
// Created by joelm on 2016-02-07.
//

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dungeon.h"
#include "globals.h"
#include "utils.h"

int saveDungeon(dungeon_t dungeon, char* fileName) { //(gridCell_t** grid, int roomCount, room_t* rooms, char* fileName) {
    uint32_t fileSize;
    uint32_t version;
    FILE* file;
    size_t bytesWritten;

    fileSize = (uint32_t)(6 + 4 + 4 + 1482 + dungeon.roomCount * 4); // magic bytes + file version + file size +
                                                                     // hardness grid + rooms

    // big endian conversion
    version = htobe32(VERSION);
    fileSize = htobe32(fileSize);

    file = fopen(fileName, "w");
    if (file == NULL) {
        // error opening the file for writing
        return -1;
    }

    // magic bytes
    bytesWritten = fwrite("RLG327", sizeof(char), 6, file);
    if (bytesWritten != 6) {
        // error writing the magic bytes
        return -2;
    }

    // file version
    bytesWritten = fwrite(&version, sizeof(version), 1, file);
    if (bytesWritten != 1) {
        // error writing the version
        return -3;
    }

    // file size
    bytesWritten = fwrite(&fileSize, sizeof(fileSize), 1, file);
    if (bytesWritten != 1) {
        // error writing the file size
        return -4;
    }

    // hardness grid
    // We do not save the immutable border
    typeof((**dungeon.grid).hardness)* hardnessPtr;
    for (int y = 0; y < HEIGHT - 2; y++) {
        for (int x = 0; x < WIDTH - 2; x++) {
            hardnessPtr = &dungeon.grid[y + 1][x + 1].hardness;
            bytesWritten = fwrite(hardnessPtr, sizeof(*hardnessPtr), 1, file);
            if (bytesWritten != 1) {
                // error writing hardness
                return -5;
            }
        }
    }

    // rooms
    room_t* roomPtr;
    for (int r = 0; r < dungeon.roomCount; r++) {
        roomPtr = &dungeon.rooms[r];
        bytesWritten = fwrite(roomPtr, sizeof(*roomPtr), 1, file);
        if (bytesWritten != 1) {
            // error writing a room
            return -6;
        }
    }
    return 0;
}

int loadDungeon(dungeon_t* dungeonPtr, char* fileName) {
    size_t bytesRead;
    char magicBytes[6];
    uint32_t version;
    uint32_t dataLen;
    // malloc2DArray could fail.  Should be checked.
    malloc2DArray((void ***) &dungeonPtr->grid, sizeof(**dungeonPtr->grid), WIDTH, HEIGHT);


    FILE* file = fopen(fileName, "r");
    if (file == NULL) {
        // error opening file
        return -1;
    }

    // read magic bytes
    bytesRead = fread(magicBytes, sizeof(char), 6, file);
    if (bytesRead < 6) {
        // error reading magic bytes
        return -2;
    }
    if (strncmp("RLG327", magicBytes, 6) != 0) {
        // invalid magic bytes
        return -3;
    }

    // read version
    bytesRead = fread(&version, sizeof(version), 1, file);
    if (bytesRead != 1) {
        // Error reading version
        return -4;
    }
    if (version != VERSION) {
        // file version is newer than this program's version
        return -5;
    }
    version = be32toh(version);

    // read data length
    bytesRead = fread(&dataLen, sizeof(dataLen), 1, file);
    if (bytesRead != 1) {
        // error reading data len
        return -6;
    }
    dataLen = be32toh(dataLen);
    if (dataLen < 1482) {
        // data is too short for the grid
        return -7;
    }

    // place the solid border
    for (int x = 0; x < WIDTH; x++) {
        dungeonPtr->grid[0][x].hardness = 255;
        dungeonPtr->grid[0][x].material = rock;
        dungeonPtr->grid[HEIGHT - 1][x].hardness = 255;
        dungeonPtr->grid[HEIGHT - 1][x].material = rock;
    }
    for (int y = 0; y < HEIGHT; y++) {
        dungeonPtr->grid[y][0].hardness = 255;
        dungeonPtr->grid[y][0].material = rock;
        dungeonPtr->grid[y][WIDTH - 1].hardness = 255;
        dungeonPtr->grid[y][WIDTH - 1].material = rock;
    }

    // read the hardness grid
    for (int y = 0; y < HEIGHT - 2; y++) {
        for (int x = 0; x < WIDTH - 2; x++) {
            bytesRead = fread(&dungeonPtr->grid[y + 1][x + 1].hardness, sizeof((**dungeonPtr->grid).hardness), 1, file);
            if (bytesRead != 1) {
                // error reading hardness data
                return -8;
            }
            if (dungeonPtr->grid[y + 1][x + 1].hardness == 0) {
                // The cell is a corridor or a room.  For now it will be a corridor.
                dungeonPtr->grid[y + 1][x + 1].material = corridor;
            } else {
                dungeonPtr->grid[y + 1][x + 1].material = rock;
            }
        }
    }

    dataLen -= (HEIGHT - 2) * (WIDTH - 2) + 14;
    if (dataLen % 4 != 0) {
        // invalid data length for rooms
        return -9;
    }

    dungeonPtr->roomCount = dataLen / 4;
    dungeonPtr->rooms = malloc(sizeof(room_t) * dungeonPtr->roomCount);
    for (int r = 0; r < dungeonPtr->roomCount; r++) {
        bytesRead = fread(&dungeonPtr->rooms[r], sizeof(room_t), 1, file);
        if (bytesRead != 1) {
            // failure reading a room
            return -10;
        }
    }
    return 0;
}

int generateDungeon(dungeon_t* dungeonPtr) {
    unsigned int seed;
    seed = (unsigned int)time(NULL); //1453848819;
    srand(seed);
    printf("Seed: %d\n", seed);

    dungeonPtr->roomCount = MIN_ROOMS + (rand() % (MAX_ROOMS - MIN_ROOMS + 1));
    printf("Room count: %d\n", dungeonPtr->roomCount);

    if (!(dungeonPtr->rooms = malloc(sizeof(room_t) * dungeonPtr->roomCount))) {
        return -1;
    }

    for (int i = 0; i < dungeonPtr->roomCount; i++) {
        generateRoom(&dungeonPtr->rooms[i], dungeonPtr->rooms, i);
    }

    if (populateGrid(dungeonPtr)) {
        return -2;
    }
    connectRooms(dungeonPtr->grid, dungeonPtr->rooms, dungeonPtr->roomCount);
    return 0;
}

void destroyDungeon(dungeon_t dungeon) {
    free2DArray((void **) dungeon.grid, HEIGHT);
    free(dungeon.rooms);
}

void dungeonPlacePC(dungeon_t* dungeonPtr) {
    int chosenRoom;
    int relX;
    int relY;

    chosenRoom = rand() % dungeonPtr->roomCount;
    relX = rand() % dungeonPtr->rooms[chosenRoom].width;
    relY = rand() % dungeonPtr->rooms[chosenRoom].height;
    dungeonPtr->PCX = relX + dungeonPtr->rooms[chosenRoom].x;
    dungeonPtr->PCY = relY + dungeonPtr->rooms[chosenRoom].y;
}

void printRooms(int roomCount, room_t* rooms) {
    for (int i = 0; i < roomCount; i++) {
        printf("rooms[%d]: (%d, %d) (%dx%d)\n", i, rooms[i].x, rooms[i].y, rooms[i].width, rooms[i].height);
    }
}

int roomDist(room_t room1, room_t room2) {
    return (int)sqrt(pow(abs(room1.x - room2.x), 2) + pow(abs(room1.y - room2.y), 2));
}

void connectRooms(gridCell_t **grid, room_t* rooms, int roomCount) {
    connectTwoRooms(grid, rooms[0], rooms[1]);
    int leastDist, leastDistI;
    int tmpDist;
    for (int i = 2; i < roomCount; i++) {
        leastDistI = 0;
        leastDist = roomDist(rooms[0], rooms[i]);
        for (int j = 1; j < i; j++) {
            tmpDist = roomDist(rooms[j], rooms[i]);
            if (tmpDist < leastDist) {
                leastDist = tmpDist;
                leastDistI = j;
            }
        }
        connectTwoRooms(grid, rooms[i], rooms[leastDistI]);
    }
}

void connectTwoRooms(gridCell_t **grid, room_t room1, room_t room2) {
    int targetX, targetY;
    int curX, curY;
    direction_t desiredDir;

    curX = room1.x + (rand() % room1.width);
    curY = room1.y + (rand() % room1.height);
    targetX = room2.x + (rand() % room2.width);
    targetY = room2.y + (rand() % room2.height);

    do {
        desiredDir = calculateDirection(curX, curY, targetX, targetY);
        if (desiredDir == northeast) {
            if (grid[curY - 1][curX].hardness < grid[curY][curX + 1].hardness) {
                curY--;
            } else {
                curX++;
            }
        } else if (desiredDir == southeast) {
            if (grid[curY + 1][curX].hardness < grid[curY][curX + 1].hardness) {
                curY++;
            } else {
                curX++;
            }
        } else if (desiredDir == southwest) {
            if (grid[curY + 1][curX].hardness < grid[curY][curX - 1].hardness) {
                curY++;
            } else {
                curX--;
            }
        } else if (desiredDir == northwest) {
            if (grid[curY - 1][curX].hardness < grid[curY][curX - 1].hardness) {
                curY--;
            } else {
                curX--;
            }
        } else {
            if (desiredDir & north) {
                curY--;
            } else if (desiredDir & south) {
                curY++;
            } else if (desiredDir & west) {
                curX--;
            } else if (desiredDir & east) {
                curX++;
            }
        }
        if (grid[curY][curX].material == rock) {
            grid[curY][curX].material = corridor;
            grid[curY][curX].hardness = 0;
        }
    } while (desiredDir != nowhere);
}

direction_t calculateDirection(int x, int y, int targetX, int targetY) {
    direction_t dir = nowhere;
    if (targetX > x) {
        dir |= east;
    } else if (targetX < x) {
        dir |= west;
    }
    if (targetY > y) {
        dir |= south;
    } else if (targetY < y) {
        dir |= north;
    }
    return dir;
}

int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount) {
    int generationTry = 0;
    room_t room;
    do {
        room.x = (uint8_t)(1 + rand() % (WIDTH - 2)); // 2 for the immutable border
        room.y = (uint8_t)(1 + rand() % (HEIGHT - 2));
        room.width = (uint8_t)(ROOM_WIDTH_MIN + rand() % (ROOM_WIDTH_MAX - ROOM_WIDTH_MIN));
        room.height = (uint8_t)(ROOM_HEIGHT_MIN + rand() % (ROOM_HEIGHT_MAX - ROOM_HEIGHT_MIN));
        if (generationTry++ >= 3000) {
            return -1;
        }
    } while (validateRoom(rooms, roomCount, room));
    *generatedRoom = room;
    return 0;
}

int validateRoom(room_t* rooms, int roomCount, room_t room) {
    if (room.x + room.width > WIDTH - 1 ||
        room.y + room.height > HEIGHT - 1) {
        return -1;
    }
    for (int i = 0; i < roomCount; i++) {
        if (validateTwoRooms(rooms[i], room) != 0) {
            return -1;
        }
    }
    return 0;
}

int validateTwoRooms(room_t room1, room_t room2) {
    room_t boundingRoom1;

    boundingRoom1.x = (uint8_t)(room1.x - 1);
    boundingRoom1.y = (uint8_t)(room1.y - 1);
    boundingRoom1.height = (uint8_t)(room1.height + 2);
    boundingRoom1.width = (uint8_t)(room1.width + 2);

    // Check that room2 is outside of room1 in each direction.
    if (room2.y + room2.height < boundingRoom1.y || // above
        room2.y > boundingRoom1.y + boundingRoom1.height || // below
        room2.x + room2.width < boundingRoom1.x || // left
        room2.x > boundingRoom1.x + boundingRoom1.width /* right */) {
        return 0;
    }
    return -1;
}

void printDungeon(dungeon_t* dungeonPtr) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == dungeonPtr->PCX && y == dungeonPtr->PCY) {
                printf("@");
            } else {
                printf("%c", (char) dungeonPtr->grid[y][x].material);
            }
        }
        printf("\n");
    }
}

int populateGrid(dungeon_t* dungeonPtr) {
    if (malloc2DArray((void ***) &dungeonPtr->grid, sizeof(**dungeonPtr->grid), WIDTH, HEIGHT)) {
        return -1;
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            dungeonPtr->grid[y][x].material = rock;
            dungeonPtr->grid[y][x].hardness = (uint8_t)(rand() % (ROCK_HARDNESS_MAX - 1) + 1);
        }
    }
    populateRooms(*dungeonPtr);
    return 0;
}

void populateRooms(dungeon_t dungeon) {
    int x, y;
    for (int i = 0; i < dungeon.roomCount; i++) {
        for (int yOffset = 0; yOffset < dungeon.rooms[i].height; yOffset++) {
            for (int xOffset = 0; xOffset < dungeon.rooms[i].width; xOffset++) {
                x = dungeon.rooms[i].x + xOffset;
                y = dungeon.rooms[i].y + yOffset;
                dungeon.grid[y][x].material = room;
                dungeon.grid[y][x].hardness = 0;
            }
        }
    }
}