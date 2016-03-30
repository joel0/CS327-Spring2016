//
// Created by joelm on 2016-01-25.
//

#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>
#include <vector>
#include <sstream>

#include "main.h"
#include "dungeon.h"
#include "path.h"
#include "turn.h"
#include "screen.h"
#include "utils.h"

bool parseFile(dungeon_t* dungeonPtr);
std::string monsterDescFileName();

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

    return parseFile(&dungeon);

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

bool parseFile(dungeon_t* dungeonPtr) {
    std::ifstream f(monsterDescFileName().c_str());
    std::string str;
    std::vector<monster_evil> monsters;
    monster_evil* tempMonsterPtr;
    std::vector<monster_evil>::iterator monster_iterator;

    std::getline(f, str);
    util_remove_cr(str);
    // The \357\273\277 is the UTF-8 header.  Some Windows programs will append it at the start of a file.
    if (str != "RLG327 MONSTER DESCRIPTION 1" && str != "\357\273\277RLG327 MONSTER DESCRIPTION 1") {
        return false;
    }

    while (!f.eof()) {
        tempMonsterPtr = monster_evil::try_parse(f);
        if (tempMonsterPtr != NULL) {
            monsters.push_back(*tempMonsterPtr);
        }
    }

    monster_iterator = monsters.begin();
    while (monster_iterator != monsters.end()) {
        std::cout << "Name: " << monster_iterator->name << std::endl;
        std::cout << "Description: " << std::endl << monster_iterator->description << std::endl;
        std::cout << "Color: " << monster_iterator->color << std::endl;
        std::cout << "Speed: " << monster_iterator->speedPtr->toString() << std::endl;
        std::cout << "Abilities: " << monster_iterator->abilities << std::endl;
        std::cout << "Hitpoints: " << monster_iterator->HP << std::endl;
        std::cout << "Attack Damage: " << monster_iterator->DAM << std::endl;
        std::cout << std::endl;
        monster_iterator = monsters.erase(monster_iterator);
    }

    f.close();
    return true;
}

void showUsage(char* name) {
    printf("Usage: %s [--save|--load] [--nummon num]\n\n", basename(name));
    printf("\t--save\tSaves a randomly generated dungeon to ~/.rlg327/dungeon\n");
    printf("\t--load\tLoads ~/.rgl327/dungeon and displays it\n");
    printf("\t--nummon\tSpecifies the number of monitors to place in the dungeon.\n");
}

char* dungeonFileName() {
    char* fullPath;
    char* homeDir;
    const char *relativePath = "/.rlg327/dungeon";

    homeDir = getenv("HOME");
    fullPath = (char *) malloc(sizeof(char) * (strlen(homeDir) + strlen(relativePath) + 1));
    sprintf(fullPath, "%s/.rlg327/dungeon", homeDir);
    return fullPath;
}

std::string monsterDescFileName() {
    std::ostringstream out;
    char* homeDir;
    const char *relativePath = "/.rlg327/monster_desc.txt";

    homeDir = getenv("HOME");
    out << homeDir << relativePath;
    return out.str();
}