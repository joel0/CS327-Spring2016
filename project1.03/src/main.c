//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "dungeon.h"

int main(int argc, char* argv[]) {
    int errLevel;
    gridCell_t** dungeonGrid;
    room_t* rooms;
    int roomCount;
    // parse arguments
    int save = 0;
    int load = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load = 1;
        } else {
            showUsage(argv[0]);
            return 0;
        }
    }

    // load or generate dungeon
    if (load) {
        errLevel = loadDungeon(&dungeonGrid, &roomCount, &rooms, dungeonFileName());
        if (errLevel) {
            printf("Failed to load the dungeon.  Read error %d\n", errLevel);
            return -1;
        }
        populateRooms(dungeonGrid, rooms, roomCount);
    } else {
        roomCount = generateDungeon(&dungeonGrid, &rooms);
        if (roomCount < 0) {
            printf("Failed to allocate memory for the dungeon grid.\n");
            return roomCount;
        }
    }

    // print dungeon
    printRooms(roomCount, rooms);
    printGrid(dungeonGrid);

    // save dungeon
    if (save) {
        errLevel = saveDungeon(dungeonGrid, roomCount, rooms, dungeonFileName());
        if (errLevel) {
            printf("Failed to save the dungeon.  Save error %d\n", errLevel);
            return -1;
        }
    }

    // Clean up
    destroyDungeon(dungeonGrid);
    free(rooms);
    return 0;
}

