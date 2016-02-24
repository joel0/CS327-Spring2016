//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>
#include <sys/unistd.h>

#include "main.h"
#include "dungeon.h"
#include "path.h"
#include "movement.h"
#include "turn.h"

int main(int argc, char* argv[]) {
    int errLevel;
    dungeon_t dungeon;
    int numMonSpecified = 0;

    //init random
    unsigned int seed;
    seed = 1456266256; //(unsigned int)time(NULL); //1453848819;
    srand(seed);
    printf("Seed: %d\n", seed);

    // parse arguments
    int save = 0;
    int load = 0;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--save") == 0) {
            save = 1;
        } else if (strcmp(argv[i], "--load") == 0) {
            load = 1;
        } else if (strcmp(argv[i], "--nummon") == 0) {
            if (i + 1 >= argc) {
                showUsage(argv[0]);
                return 0;
            }
            dungeon.monsterCount = atoi(argv[++i]);
            numMonSpecified = 1;
        } else {
            showUsage(argv[0]);
            return 0;
        }
    }

    if (!numMonSpecified) {
        dungeon.monsterCount = rand() % (42 / 2);
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

    // make calculations
    pathNontunneling(&dungeon);
    pathTunneling(&dungeon);

    // print dungeon
    printRooms(dungeon.roomCount, dungeon.rooms);
    printMonsters(dungeon.monsterCount, dungeon.monsterPtrs);
    printDungeon(&dungeon);

    // do move
    while (dungeon.PC.alive && dungeon.monsterCount) {
        //usleep(400000);
        //movePC(&dungeon);
        int PCTurn = turnIsPC(&dungeon);
        turnDo(&dungeon);
        if (PCTurn) {
            printDungeon(&dungeon);
        }
    }

    if (!dungeon.PC.alive) {
        printf("You died!\n");
    } else {
        printf("Yay!  You defeated all the monsters.\n");
    }

    //printDistGrid(&dungeon, dungeon.tunnelingDist);
    //printDistGrid(&dungeon, dungeon.nontunnelingDist);

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

void showUsage(char* name) {
    printf("Usage: %s [--save|--load]\n\n", basename(name));
    printf("\t--save\tSaves a randomly generated dungeon to ~/.rlg327/dungeon\n");
    printf("\t--load\tLoads ~/.rgl327/dungeon and displays it\n");
}

char* dungeonFileName() {
    char* fullPath;
    char* homeDir;
    char* relativePath = "/.rlg327/dungeon";

    homeDir = getenv("HOME");
    fullPath = malloc(sizeof(char) * (strlen(homeDir) + strlen(relativePath) + 1));
    sprintf(fullPath, "%s/.rlg327/dungeon", homeDir);
    return fullPath;
}