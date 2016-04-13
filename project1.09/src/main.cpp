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

bool parse_monster_descrip_file(std::vector<monster_descrip*> &monster_descrips);
bool parse_item_descrip_file(std::vector<item_descrip*>& item_descrips);
std::string itemDescFileName();
std::string monsterDescFileName();

int main(int argc, char* argv[]) {
    int errLevel;
    dungeon_t dungeon;
    std::vector<monster_descrip*> monster_descrips;
    std::vector<item_descrip*> item_descrips;
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

    if (!parse_monster_descrip_file(monster_descrips)) {
        std::cout << "Error parsing the monster descriptions." << std::endl;
        return -1;
    }
    if (!parse_item_descrip_file(item_descrips)) {
        std::cout << "Error parsing the object descriptions." << std::endl;
        return -1;
    }

    if (!numMonSpecified) {
        dungeon.monsterCount = rand() % (42 / 3);
    }

    // load or generate dungeon
    if (load) {
        errLevel = loadDungeon(&dungeon, dungeonFileName(), monster_descrips, item_descrips);
        if (errLevel) {
            printf("Failed to load the dungeon.  Read error %d\n", errLevel);
            return -1;
        }
        populateRooms(dungeon);
    } else {
        errLevel = generateDungeon(&dungeon, monster_descrips, item_descrips);
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
    while (dungeon.PCPtr->alive && dungeon.monsterCount > 1 && userAction != actionSave) {
        int PCTurn = turnIsPC(&dungeon);
        if (PCTurn) {
            dungeon.PCPtr->updateGridKnown(dungeon.grid);
            printDungeon(dungeon.PCPtr->gridKnown);

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
                    errLevel = generateDungeon(&dungeon, monster_descrips, item_descrips);
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
                case actionListInventory:
                    dungeon.PCPtr->show_inventory(true);
                    break;
                case actionListEquipment:
                    dungeon.PCPtr->show_equipment(true);
                    break;
                case actionDropItem:
                    dungeon.PCPtr->drop_item(dungeon);
                    break;
                case actionInspectItem:
                    dungeon.PCPtr->inspect_item();
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
    } else if (!dungeon.PCPtr->alive) {
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
    delete_descrips(monster_descrips);
    delete_descrips(item_descrips);
    return 0;
}

bool parse_monster_descrip_file(std::vector<monster_descrip *> &monster_descrips) {
    std::ifstream f(monsterDescFileName().c_str());
    std::string str;
    //std::vector<monster_descrip*> monsters;
    monster_descrip* tempMonsterPtr;
    std::vector<monster_descrip*>::iterator monster_iterator;

    std::getline(f, str);
    util_remove_cr(str);
    // The \357\273\277 is the UTF-8 header.  Some Windows programs will append it at the start of a file.
    if (str != "RLG327 MONSTER DESCRIPTION 1" && str != "\357\273\277RLG327 MONSTER DESCRIPTION 1") {
        return false;
    }

    while (!f.eof()) {
        try {
            tempMonsterPtr = new monster_descrip(f);
            monster_descrips.push_back(tempMonsterPtr);
        } catch (std::string ex) {
            std::cout << ex << std::endl;
        } catch (const char* ex) {
            // EOF is expected when the file contains no more monsters
            if (strcmp(ex, "EOF") != 0) {
                std::cout << ex << std::endl;
            }
        }
    }

    monster_iterator = monster_descrips.begin();
    while (monster_iterator != monster_descrips.end()) {
        std::cout << (*monster_iterator)->to_string() << std::endl;
        //delete *monster_iterator;
        monster_iterator++;
    }

    f.close();
    return true;
}

bool parse_item_descrip_file(std::vector<item_descrip*>& item_descrips) {
    std::ifstream f(itemDescFileName().c_str());
    std::string str;
    item_descrip* tempItemPtr;
    std::vector<item_descrip*>::iterator item_iterator;

    std::getline(f, str);
    util_remove_cr(str);
    // The \357\273\277 is the UTF-8 header.  Some Windows programs will append it at the start of a file.
    if (str != "RLG327 OBJECT DESCRIPTION 1" && str != "\357\273\277RLG327 OBJECT DESCRIPTION 1") {
        return false;
    }

    while (!f.eof()) {
        try {
            tempItemPtr = new item_descrip(f);
            item_descrips.push_back(tempItemPtr);
        } catch (std::string ex) {
            std::cout << ex << std::endl;
        } catch  (const char* ex) {
            // EOF is expected when the file contains no more objects
            if (strcmp(ex, "EOF") != 0) {
                std::cout << ex << std::endl;
            }
        }
    }

    item_iterator = item_descrips.begin();
    while (item_iterator != item_descrips.end()) {
        std::cout << (*item_iterator)->to_string() << std::endl;
        item_iterator++;
    }

    f.close();
    return true;
}

template<typename T> void delete_descrips(std::vector<T*>& descrips) {
    typename std::vector<T*>::iterator item_iterator;
    item_iterator = descrips.begin();
    while (item_iterator != descrips.end()) {
        delete *item_iterator;
        item_iterator++;
    }
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

std::string itemDescFileName() {
    std::ostringstream out;
    char* homeDir;
    const char *relativePath = "/.rlg327/object_desc.txt";

    homeDir = getenv("HOME");
    out << homeDir << relativePath;
    return out.str();
}