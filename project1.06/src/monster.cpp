//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <curses.h>

#include "monster.h"
#include "dungeon.h"

char* monster::toString(dungeon_t* dungeonPtr) {
    int offX = ((monster&) dungeonPtr->PC).x - x;
    int offY = ((monster&) dungeonPtr->PC).y - y;
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

monster_evil::monster_evil() {
    speed = MONSTER_MIN_SPEED + (rand() % (MONSTER_MAX_SPEED - MONSTER_MIN_SPEED + 1));
    type = (uint8_t) (rand() & 0x0F); // 50% chance of each bit being 1
    lastPCX = 0;
    lastPCY = 0;
    alive = true;
    x = 0;
    y = 0;
}

char monster_evil::getChar() {
    if (type < 10) {
        return '0' + type;
    }
    return (char) ('a' + type - 10);
}

// To keep track of how many monsters to free in the destroy
static int totalMonsters;

void initMonsters(dungeon_t* dungeonPtr) {
    monster* randMonsterPtr;
    dungeonPtr->PC = (monster_t) new monster_PC(0, 0);
    dungeonRandomlyPlaceMonster(dungeonPtr, &dungeonPtr->PC);
    dungeonPtr->monsterCount++; // +1 for the PC
    dungeonPtr->monsterPtrs = (monster_t**) malloc(sizeof(monster_t*) * dungeonPtr->monsterCount);
    dungeonPtr->monsterPtrs[0] = &dungeonPtr->PC;

    for (int i = 1; i < dungeonPtr->monsterCount; i++) {
        //randMonsterPtr = (monster_t*) malloc(sizeof(monster_t));
        randMonsterPtr = new monster_evil();
        //monsterGenerate(randMonsterPtr);
        dungeonPtr->monsterPtrs[i] = (monster_t*) randMonsterPtr;
        dungeonRandomlyPlaceMonster(dungeonPtr, (monster_t*) randMonsterPtr);
    }
    totalMonsters = dungeonPtr->monsterCount;
}

void monstersDestroy(dungeon_t* dungeonPtr) {
    for (int i = 1; i < totalMonsters; i++) {
        free(dungeonPtr->monsterPtrs[i]);
    }
    free(dungeonPtr->monsterPtrs);
}

//void monsterGenerate(monster_t* m) {
//    m->isPC = 0;
//    m->speed = MONSTER_MIN_SPEED + (rand() % (MONSTER_MAX_SPEED - MONSTER_MIN_SPEED + 1));
//    m->type = (uint8_t) (rand() & 0x0F); // 50% chance of each bit being 1
//    m->lastPCX = 0;
//    m->lastPCY = 0;
//    m->alive = 1;
//    m->x = 0;
//    m->y = 0;
//}

void monsterList(dungeon_t* dungeonPtr) {
    int exit = 0;
    int userChar;
    char* monsterDescrip;
    int offset = 0;
    int maxOffset = dungeonPtr->monsterCount - 1 + 4 - 11;
    WINDOW* monsterWin = newpad(dungeonPtr->monsterCount - 1 + 4, 60);
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    wbkgd(monsterWin, COLOR_PAIR(1));
    wborder(monsterWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(monsterWin, 1, 60 / 2 - 8 / 2, "MONSTERS");
    wmove(monsterWin, 2, 1);
    whline(monsterWin, ACS_HLINE, 58);
    for (int i = 0; i < dungeonPtr->monsterCount - 1; i++) {
        monsterDescrip = monsterDescription(dungeonPtr, dungeonPtr->monsterPtrs[i + 1]);
        mvwaddstr(monsterWin, i + 3, 1, monsterDescrip);
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

char monsterGetChar(monster_t* monsterPtr) {
    return ((monster&) *monsterPtr).getChar();
}

char* monsterDescription(dungeon_t* dungeonPtr, monster_t* monsterPtr) {
    return ((monster&) *monsterPtr).toString(dungeonPtr);
}

int monsterGetX(monster_t* monsterPtr) { return ((monster&) *monsterPtr).x; }
int monsterGetY(monster_t* monsterPtr) { return ((monster&) *monsterPtr).y; }

int monsterGetLastPCX(monster_t* monsterPtr) { return ((monster&) *monsterPtr).lastPCX; }
int monsterGetLastPCY(monster_t* monsterPtr) { return ((monster&) *monsterPtr).lastPCY; }

void monsterSetX(monster_t* monsterPtr, int x) { ((monster&) *monsterPtr).x = x; }
void monsterSetY(monster_t* monsterPtr, int y) { ((monster&) *monsterPtr).y = y; }

void monsterSetLastPCX(monster_t* monsterPtr, int x) { ((monster&) *monsterPtr).lastPCX = x; }
void monsterSetLastPCY(monster_t* monsterPtr, int y) { ((monster&) *monsterPtr).lastPCY = y; }

int monsterIsAlive(monster_t* monsterPtr) { return ((monster&) *monsterPtr).alive; }

int monsterIsPC(monster_t* monsterPtr) { return ((monster&) *monsterPtr).isPC(); }

int monsterSpeed(monster_t* monsterPtr) { return ((monster&) *monsterPtr).speed; }

void monsterKill(monster_t* monsterPtr) { ((monster&) *monsterPtr).alive = false; }

uint8_t monsterGetType(monster_t* monsterPtr) { return ((monster_evil&) *monsterPtr).type; }