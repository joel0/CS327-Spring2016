//
// Created by joelm on 2016-01-25.
//

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

int main(int argc, char* argv[]) {
    int roomCount;
    unsigned int seed = (unsigned int)time(NULL); //1453848819;
    srand(seed);
    printf("Seed: %d\n", seed);

    roomCount = MIN_ROOMS + (rand() % (MAX_ROOMS - MIN_ROOMS + 1));
    printf("Room count: %d\n", roomCount);

    room_t rooms[roomCount];

    for (int i = 0; i < roomCount; i++) {
        generateRoom(&rooms[i], rooms, i);
    }

    for (int i = 0; i < roomCount; i++) {
        printf("rooms[%d]: (%d, %d) (%dx%d)\n", i, rooms[i].x, rooms[i].y, rooms[i].width, rooms[i].height);
    }

    gridCell_t** dungeonGrid = populateGrid(rooms, roomCount);
    connectRooms(dungeonGrid, rooms, roomCount);
    printGrid(dungeonGrid);
    free2DGrid(dungeonGrid, HEIGHT);

    return 0;
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
        if ((curX + curY) % 2) {
            if (desiredDir & north) {
                curX--;
            } else if (desiredDir & south) {
                curX++;
            } else if (desiredDir & west) {
                curY--;
            } else if (desiredDir & east) {
                curY++;
            }
        } else {
            if (desiredDir & east) {
                curY++;
            } else if (desiredDir & west) {
                curY--;
            } else if (desiredDir & south) {
                curX++;
            } else if (desiredDir & north) {
                curX--;
            }
        }
        if (grid[curY][curX].material == rock) {
            grid[curY][curX].material = corridor;
            grid[curY][curX].hardness = -1;
        }
    } while (desiredDir != nowhere);
}

direction_t calculateDirection(int x, int y, int targetX, int targetY) {
    direction_t dir = nowhere;
    if (targetX > x) {
        dir |= south;
    } else if (targetX < x) {
        dir |= north;
    }
    if (targetY > y) {
        dir |= east;
    } else if (targetY < y) {
        dir |= west;
    }
    return dir;
}

int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount) {
    int generationTry = 0;
    room_t room;
    do {
        room.x = 1 + rand() % (WIDTH - 2); // 2 for the immutable border
        room.y = 1 + rand() % (HEIGHT - 2);
        room.width = ROOM_WIDTH_MIN + rand() % (ROOM_WIDTH_MAX - ROOM_WIDTH_MIN);
        room.height = ROOM_HEIGHT_MIN + rand() % (ROOM_HEIGHT_MAX - ROOM_HEIGHT_MIN);
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

    boundingRoom1.x = room1.x - 1;
    boundingRoom1.y = room1.y - 1;
    boundingRoom1.height = room1.height + 2;
    boundingRoom1.width = room1.width + 2;

    // Check that room2 is outside of room1 in each direction.
    if (room2.y + room2.height < boundingRoom1.y || // above
            room2.y > boundingRoom1.y + boundingRoom1.height || // below
            room2.x + room2.width < boundingRoom1.x || // left
            room2.x > boundingRoom1.x + boundingRoom1.width /* right */) {
        return 0;
    }
    return -1;
}

void printGrid(gridCell_t** grid) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            printf("%c", (char)grid[y][x].material);
        }
        printf("\n");
    }
}

gridCell_t** populateGrid(room_t* rooms, int roomCount) {
    gridCell_t **grid;

    if (malloc2DGrid(&grid, WIDTH, HEIGHT)) {
        return NULL;
    }
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            grid[y][x].material = rock;
            grid[y][x].hardness = rand() % ROCK_HARDNESS_MAX;
        }
    }
    populateRooms(grid, rooms, roomCount);
    return grid;
}

void populateRooms(gridCell_t** grid, room_t* rooms, int roomCount) {
    int x, y;
    for (int i = 0; i < roomCount; i++) {
        for (int yOffset = 0; yOffset < rooms[i].height; yOffset++) {
            for (int xOffset = 0; xOffset < rooms[i].width; xOffset++) {
                x = rooms[i].x + xOffset;
                y = rooms[i].y + yOffset;
                grid[y][x].material = room;
                grid[y][x].hardness = -1;
            }
        }
    }
}

int malloc2DGrid(gridCell_t*** grid, int width, int height) {
    if (!(*grid = malloc(height * sizeof(gridCell_t*)))) {
        return -1;
    }

    for (int y = 0; y < height; y++) {
        if (!((*grid)[y] = malloc(width * sizeof(gridCell_t)))) {
            return -1;
        }
    }
    return 0;
}

void free2DGrid(gridCell_t** grid, int height) {
    for (int y = 0; y < height; y++) {
        free(grid[y]);
    }
    free(grid);
}