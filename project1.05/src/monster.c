//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <curses.h>

#include "monster.h"
#include "dungeon.h"

// To keep track of how many monsters to free in the destroy
static int totalMonsters;

void initMonsters(dungeon_t* dungeonPtr) {
    monster_t* randMonsterPtr;
    dungeonPtr->PC.isPC = 1;
    dungeonPtr->PC.speed = 10;
    dungeonPtr->PC.alive = 1;
    dungeonPtr->PC.type = MONSTER_TUNNELING;
    dungeonRandomlyPlaceMonster(dungeonPtr, &dungeonPtr->PC);
    dungeonPtr->monsterCount++; // +1 for the PC
    dungeonPtr->monsterPtrs = malloc(sizeof(monster_t*) * dungeonPtr->monsterCount);
    dungeonPtr->monsterPtrs[0] = &dungeonPtr->PC;

    for (int i = 1; i < dungeonPtr->monsterCount; i++) {
        randMonsterPtr = malloc(sizeof(monster_t));
        monsterGenerate(randMonsterPtr);
        dungeonPtr->monsterPtrs[i] = randMonsterPtr;
        dungeonRandomlyPlaceMonster(dungeonPtr, randMonsterPtr);
    }
    totalMonsters = dungeonPtr->monsterCount;
}

void monstersDestroy(dungeon_t* dungeonPtr) {
    for (int i = 1; i < totalMonsters; i++) {
        free(dungeonPtr->monsterPtrs[i]);
    }
    free(dungeonPtr->monsterPtrs);
}

char monsterGetChar(monster_t m) {
    if (m.isPC) {
        return '@';
    }
    if (m.type < 10) {
        return (char) ('0' + m.type);
    }
    return (char) ('a' + m.type - 10);
}

void monsterGenerate(monster_t* m) {
    m->isPC = 0;
    m->speed = MONSTER_MIN_SPEED + (rand() % (MONSTER_MAX_SPEED - MONSTER_MIN_SPEED + 1));
    m->type = (uint8_t) (rand() & 0x0F); // 50% chance of each bit being 1
    m->lastPCX = 0;
    m->lastPCY = 0;
    m->alive = 1;
    m->x = 0;
    m->y = 0;
}

void monsterList(dungeon_t* dungeonPtr) {
    int exit = 0;
    int userChar;
    char* monsterDescrip;
    int offset = 0;
    int maxOffset = dungeonPtr->monsterCount - 1 + 4 - 11;
    WINDOW* monsterWin = newpad(dungeonPtr->monsterCount - 1 + 4, 60);
//    WINDOW* monsterWin = newwin(10, 60, HEIGHT / 2 - (10 / 2), WIDTH / 2 - (60 / 2));
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
    //wrefresh(monsterWin);
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

char* monsterDescription(dungeon_t* dungeonPtr, monster_t* monster) {
    int offX = dungeonPtr->PC.x - monster->x;
    int offY = dungeonPtr->PC.y - monster->y;
    char* returnStr = malloc(30 * sizeof(char));
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
        sprintf(returnStr, "%c, %s and %s", monsterGetChar(*monster), verticalPart, horizontalPart);
    } else {
        sprintf(returnStr, "%c, %s%s", monsterGetChar(*monster), verticalPart, horizontalPart);
    }
    return returnStr;
}