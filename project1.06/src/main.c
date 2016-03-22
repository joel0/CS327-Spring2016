//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

#include "main.h"
#include "dungeon.h"
#include "path.h"
#include "turn.h"
#include "screen.h"

int main(int argc, char* argv[]) {
    int errLevel;
    dungeon_t dungeon;
    int numMonSpecified = 0;

    //init random
    unsigned int seed;
    seed = (unsigned int)time(NULL); //1453848819;
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

    // save dungeon
    if (save) {
        errLevel = saveDungeon(dungeon, dungeonFileName());
        if (errLevel) {
            printf("Failed to save the dungeon.  Save error %d\n", errLevel);
            return -1;
        }
    }

    // make calculations
    pathTunneling(&dungeon);
    pathNontunneling(&dungeon);

    // print dungeon
    initTerminal();
    printDungeon(dungeon.grid);

    // do move
    PC_action userAction = actionMovement;
    while (monsterIsAlive(dungeon.PCPtr) && dungeon.monsterCount > 1 && userAction != actionSave) {
        int PCTurn = turnIsPC(&dungeon);
        if (PCTurn) {
            monsterUpdatePCGridKnown(dungeon.PCPtr, dungeon.grid);
            printDungeon(monsterGetPCGridKnown(dungeon.PCPtr));

            userAction = turnDoPC(&dungeon);
            switch (userAction) {
                case actionStairsUp:
                case actionStairsDn:
                    // Destroy old dungeon
                    pathFreeDistGrid(dungeon.nontunnelingDist);
                    pathFreeDistGrid(dungeon.tunnelingDist);
                    destroyDungeon(dungeon);
                    dungeon.monsterCount--; // PC was counted as a "monster".  He must be removed for reinitialisation.
                    // Build new dungeon
                    errLevel = generateDungeon(&dungeon);
                    if (errLevel) {
                        endwin();
                        printf("Failed to allocate memory for the dungeon grid.\n");
                        return errLevel;
                    }
                    pathMallocDistGrid(&dungeon.tunnelingDist);
                    pathMallocDistGrid(&dungeon.nontunnelingDist);
                    pathTunneling(&dungeon);
                    pathNontunneling(&dungeon);
                    break;
                case actionListMonsters:
                    monsterList(&dungeon);
                    break;
                default: break;
            }
        } else {
            turnDo(&dungeon);
        }
    }

    screenClearRow(0);
    if (userAction == actionSave) {
        mvprintw(0, 0, "Game saved (haha, not really!).  Press any key to exit.");
    } else if (!monsterIsAlive(dungeon.PCPtr)) {
        mvprintw(0, 0, "You died!  Press any key to exit.");
    } else {
        mvprintw(0, 0, "Yay!  You defeated all the monsters.  Press any key to exit.");
    }

    // Clean up
    getch(); // "press any key"
    endwin();
    pathFreeDistGrid(dungeon.nontunnelingDist);
    pathFreeDistGrid(dungeon.tunnelingDist);
    destroyDungeon(dungeon);
    return 0;
}

void showUsage(char* name) {
    printf("Usage: %s [--save|--load]\n\n", basename(name));
    printf("\t--save\tSaves a randomly generated dungeon to ~/.rlg327/dungeon\n");
    printf("\t--load\tLoads ~/.rgl327/dungeon and displays it\n");
    printf("\t--nummon\tSpecifies the number of monitors to place in the dungeon.\n");
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