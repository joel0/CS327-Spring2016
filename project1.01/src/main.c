//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "main.h"

int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount);
int validateRoom(room_t* rooms, int roomCount, room_t room);
int validateTwoRooms(room_t room1, room_t room2);
void printGrid(gridCell_t** grid);
gridCell_t** populateGrid(room_t* rooms, int roomCount);
void populateRooms(gridCell_t** grid, room_t* rooms, int roomCount);
int malloc2DGrid(gridCell_t*** grid, int width, int height);
void free2DGrid(gridCell_t** grid, int height);

int main(int argc, char* argv[]) {
    int roomCount;
    unsigned int seed = (unsigned int)time(NULL);
    srand(seed);
    printf("Seed: %d\n", seed);

    roomCount = 5 + (rand() % 6);
    printf("Room count: %d\n", roomCount);

    room_t rooms[roomCount];
    
//    rooms[0].x = 5;
//    rooms[0].y = 5;
//    rooms[0].height = 3;
//    rooms[0].width = 4;
//    rooms[1].x = 30;
//    rooms[1].y = 10;
//    rooms[1].height = 8;
//    rooms[1].width = 5;
//    generateRoom(&rooms[0], rooms, 0);
//    generateRoom(&rooms[1], rooms, 1);
    for (int i = 0; i < roomCount; i++) {
        generateRoom(&rooms[i], rooms, i);
    }

//    printf("rooms[0]: (%d, %d) (%dx%d)\n", rooms[0].x, rooms[0].y, rooms[0].width, rooms[0].height);
//    printf("rooms[1]: (%d, %d) (%dx%d)\n", rooms[1].x, rooms[1].y, rooms[1].width, rooms[1].height);
    for (int i = 0; i < roomCount; i++) {
        printf("rooms[%d]: (%d, %d) (%dx%d)\n", i, rooms[i].x, rooms[i].y, rooms[i].width, rooms[i].height);
    }

    gridCell_t** dungeonGrid = populateGrid(rooms, roomCount);
    printGrid(dungeonGrid);
    free2DGrid(dungeonGrid, HEIGHT);

    printf("Done\n");
    return 0;
}

int generateRoom(room_t* generatedRoom, room_t* rooms, int roomCount) {
    int generationTry = 0;
    room_t room;
    do {
        room.x = 1 + rand() % (WIDTH - ROOM_WIDTH_MAX - 2); // 2 for the immutable border
        room.y = 1 + rand() % (HEIGHT - ROOM_HEIGHT_MAX - 2);
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
    boundingRoom1.width = room1.height + 2;

    for (int y2 = room2.y; y2 < room2.y + room2.height; y2++) {
        for (int x2 = room2.x; x2 < room2.x + room2.width; x2++) {
            if (x2 >= boundingRoom1.x && x2 <= boundingRoom1.x + boundingRoom1.width &&
                    y2 >= boundingRoom1.y && y2 <= boundingRoom1.y + boundingRoom1.height) {
                return -1;
            }
        }
    }
    return 0;
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
                grid[y][x].material = room1;
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