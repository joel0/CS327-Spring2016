//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <string.h>

#include "main.h"
#include "dungeon.h"
#include "path.h"

int main(int argc, char* argv[]) {
    int errLevel;
    dungeon_t dungeon;
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
        errLevel = loadDungeon(&dungeon, dungeonFileName());
        if (errLevel) {
            printf("Failed to load the dungeon.  Read error %d\n", errLevel);
            return -1;
        }
        populateRooms(dungeon);
    } else {
        errLevel = generateDungeon(&dungeon);
        if (errLevel) {
            printf("Failed to allocate memory for the dungeon grid.\n");
            return errLevel;
        }
    }
    pathMallocDistGrid(&dungeon.tunnelingDist);
    pathMallocDistGrid(&dungeon.nontunnelingDist);
    dungeon.PCX = 18;
    dungeon.PCY = 4;

    // make calculations
    pathTunneling(&dungeon);
    pathNontunneling(&dungeon);

    // print dungeon
    printRooms(dungeon.roomCount, dungeon.rooms);
    printDungeon(&dungeon);

    printDistGrid(dungeon.tunnelingDist);
    printDistGrid(dungeon.nontunnelingDist);

    // save dungeon
    if (save) {
        errLevel = saveDungeon(dungeon, dungeonFileName());
        if (errLevel) {
            printf("Failed to save the dungeon.  Save error %d\n", errLevel);
            return -1;
        }
    }

    // Clean up
    pathFreeDistGrid(dungeon.nontunnelingDist);
    pathFreeDistGrid(dungeon.tunnelingDist);
    destroyDungeon(dungeon);
    return 0;
}

