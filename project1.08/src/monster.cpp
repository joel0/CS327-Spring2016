//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <curses.h>
#include <cstring>
#include <sstream>
#include <vector>

#include "monster.h"
#include "dungeon.h"
#include "utils.h"
#include "globals.h"
#include "movement.h"
#include "item_descrip.h"

char* monster::toString(dungeon_t* dungeonPtr) {
    int offX = ((monster*) dungeonPtr->PCPtr)->x - x;
    int offY = ((monster*) dungeonPtr->PCPtr)->y - y;
    char* returnStr = (char*) malloc(30 * sizeof(char));
    char verticalPart[20] = "";
    char horizontalPart[20] = "";

    if (offY < 0) {
        //south
        sprintf(verticalPart, "%d south", -offY);
    } else if (offY > 0) {
        //north
        sprintf(verticalPart, "%d north", offY);
    }
    if (offX < 0) {
        //east
        sprintf(horizontalPart, "%d east", -offX);
    } else if (offX > 0) {
        //west
        sprintf(horizontalPart, "%d west", offX);
    }
    if (horizontalPart[0] && verticalPart[0]) {
        sprintf(returnStr, "%c, %s and %s", getChar(), verticalPart, horizontalPart);
    } else {
        sprintf(returnStr, "%c, %s%s", getChar(), verticalPart, horizontalPart);
    }
    return returnStr;
}

//monster_evil::monster_evil() {
//    //speed = MONSTER_MIN_SPEED + (rand() % (MONSTER_MAX_SPEED - MONSTER_MIN_SPEED + 1));
//    type = (uint8_t) (rand() & 0x0F); // 50% chance of each bit being 1
//    lastPCX = 0;
//    lastPCY = 0;
//    alive = true;
//    x = 0;
//    y = 0;
//}

char monster_evil::getChar() {
    return symb;
}

monster_PC::monster_PC(int x, int y) :
        monster(std::string("PC"), std::string("You"), COLOR_WHITE, 10, MONSTER_TUNNELING, 100, new dice_set(0, 0, 1), '@') {
    malloc2DArray((void***) &gridKnown, sizeof(**gridKnown), WIDTH, HEIGHT);
    for (int curY = 0; curY < HEIGHT; curY++) {
        for (int curX = 0; curX < WIDTH; curX++) {
            gridKnown[curY][curX].hardness = 0;
            gridKnown[curY][curX].material = rock;
            gridKnown[curY][curX].monsterPtr = NULL;
            gridKnown[curY][curX].itemPtr = NULL;
        }
    }
}

monster_PC::~monster_PC() {
    free2DArray((void**) gridKnown, HEIGHT);
    delete DAM_ptr;
}

void monster_PC::updateGridKnown(gridCell_t** world) {
    int minX, minY, maxX, maxY;
    // Remove any monsters that may be out of visibility
    for (int curY = 0; curY < HEIGHT; curY++) {
        for (int curX = 0; curX < WIDTH; curX++) {
            gridKnown[curY][curX].monsterPtr = NULL;
        }
    }

    minX = MAX(0, x - PC_VISION_DIST);
    minY = MAX(0, y - PC_VISION_DIST);
    maxX = MIN(WIDTH - 1, x + PC_VISION_DIST);
    maxY = MIN(HEIGHT - 1, y + PC_VISION_DIST);
    for (int curY = minY; curY <= maxY; curY++) {
        for (int curX = minX; curX <= maxX; curX++) {
            if(movementIsLineOfSight(world, x, y, curX, curY)) {
                gridKnown[curY][curX] = world[curY][curX];
            }
        }
    }
}

// To keep track of how many monsters to free in the destroy
static int totalMonsters;

void initMonsters(dungeon_t* dungeonPtr, std::vector<monster_descrip*>& monster_descrips) {
    monster* randMonsterPtr;
    dungeonPtr->PCPtr = new monster_PC(0, 0);
    dungeonRandomlyPlaceMonster(dungeonPtr, dungeonPtr->PCPtr);
    dungeonPtr->monsterCount++; // +1 for the PC
    dungeonPtr->monsterPtrs = (monster**) malloc(sizeof(monster*) * dungeonPtr->monsterCount);
    dungeonPtr->monsterPtrs[0] = dungeonPtr->PCPtr;

    for (int i = 1; i < dungeonPtr->monsterCount; i++) {
        randMonsterPtr = monster_descrips[rand() % monster_descrips.size()]->generate();
        dungeonPtr->monsterPtrs[i] = randMonsterPtr;
        dungeonRandomlyPlaceMonster(dungeonPtr, randMonsterPtr);
    }
    totalMonsters = dungeonPtr->monsterCount;
}

void initItems(dungeon_t& dungeon, std::vector<item_descrip*>& item_descrips) {
    item* randItemPtr;
    dungeon.itemCount = 10 + rand() % 10;
    dungeon.itemPtrs = (item**) malloc(sizeof(item*) * dungeon.itemCount);

    for (int i = 0; i < dungeon.itemCount; i++) {
        randItemPtr = item_descrips[rand() % item_descrips.size()]->generate();
        dungeon.itemPtrs[i] = randItemPtr;
        dungeonRandomlyPlaceItem(dungeon, *randItemPtr);
    }
}

void monstersDestroy(dungeon_t* dungeonPtr) {
    for (int i = 1; i < totalMonsters; i++) {
        delete dungeonPtr->monsterPtrs[i];
    }
    free(dungeonPtr->monsterPtrs);
    delete dungeonPtr->PCPtr;
}

void itemsDestroy(dungeon_t& dungeon) {
    for (int i = 0; i < dungeon.itemCount; i++) {
        delete dungeon.itemPtrs[i];
    }
    free(dungeon.itemPtrs);
}

void monsterList(dungeon_t* dungeonPtr) {
    int exit = 0;
    int userChar;
    char* monsterDescrip;
    int offset = 0;
    int maxOffset = dungeonPtr->monsterCount - 1 + 4 - 11;
    WINDOW* monsterWin = newpad(dungeonPtr->monsterCount - 1 + 4, 60);
    wbkgd(monsterWin, COLOR_PAIR(COLOR_INVERTED));
    wborder(monsterWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(monsterWin, 1, 60 / 2 - 8 / 2, "MONSTERS");
    wmove(monsterWin, 2, 1);
    whline(monsterWin, ACS_HLINE, 58);
    for (int i = 0; i < dungeonPtr->monsterCount - 1; i++) {
        monsterDescrip = dungeonPtr->monsterPtrs[i + 1]->toString(dungeonPtr);
        wattron(monsterWin, COLOR_PAIR(dungeonPtr->monsterPtrs[i + 1]->color + 10));
        mvwaddstr(monsterWin, i + 3, 1, monsterDescrip);
        wattroff(monsterWin, COLOR_PAIR(dungeonPtr->monsterPtrs[i + 1]->color + 10));
        free(monsterDescrip);
    }
    wnoutrefresh(stdscr);
    do {
        pnoutrefresh(monsterWin, offset, 0, 5, 10, 15, 70);
        doupdate();
        userChar = getch();
        switch (userChar) {
            case KEY_DOWN:
                offset++;
                if (offset > maxOffset) {
                    offset = maxOffset;
                }
                break;
            case KEY_UP:
                offset--;
                if (offset < 0) {
                    offset = 0;
                }
                break;
            case 27:
                exit = 1;
                break;
            default: break;
        }
    } while (!exit);
    delwin(monsterWin);
    refresh();
}

// +----------------------------------------------------+
// | C EXPORTS                                          |
// +----------------------------------------------------+

//char monsterGetChar(monster_t* monsterPtr) {
//    return ((monster*) monsterPtr)->getChar();
//}
//
//char* monsterDescription(dungeon_t* dungeonPtr, monster_t* monsterPtr) {
//    return ((monster*) monsterPtr)->toString(dungeonPtr);
//}
//
//int monsterGetX(monster_t* monsterPtr) { return ((monster*) monsterPtr)->x; }
//int monsterGetY(monster_t* monsterPtr) { return ((monster*) monsterPtr)->y; }
//
//int monsterGetLastPCX(monster_t* monsterPtr) { return ((monster*) monsterPtr)->lastPCX; }
//int monsterGetLastPCY(monster_t* monsterPtr) { return ((monster*) monsterPtr)->lastPCY; }
//
//void monsterSetX(monster_t* monsterPtr, int x) { ((monster*) monsterPtr)->x = x; }
//void monsterSetY(monster_t* monsterPtr, int y) { ((monster*) monsterPtr)->y = y; }
//
//void monsterSetLastPCX(monster_t* monsterPtr, int x) { ((monster*) monsterPtr)->lastPCX = x; }
//void monsterSetLastPCY(monster_t* monsterPtr, int y) { ((monster*) monsterPtr)->lastPCY = y; }
//
//int monsterIsAlive(monster_t* monsterPtr) { return ((monster*) monsterPtr)->alive; }
//
//int monsterIsPC(monster_t* monsterPtr) { return ((monster*) monsterPtr)->isPC(); }
//
//int monsterSpeed(monster_t* monsterPtr) { return ((monster*) monsterPtr)->speedPtr->roll(); }
//
//void monsterKill(monster_t* monsterPtr) { ((monster*) monsterPtr)->alive = false; }
//
//uint8_t monsterGetType(monster_t* monsterPtr) { return ((monster_evil*) monsterPtr)->type; }
//
//void monsterUpdatePCGridKnown(monster_t* monsterPtr, gridCell_t** world) {
//    ((monster_PC*) monsterPtr)->updateGridKnown(world);
//}
//
//gridCell_t** monsterGetPCGridKnown(monster_t* monsterPtr) {
//    return ((monster_PC*) monsterPtr)->gridKnown;
//}