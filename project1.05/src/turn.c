//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>

#include "turn.h"
#include "movement.h"

int turnCompare(const void* d1, const void* d2);
void turnDelete(void* d);

void turnInit(dungeon_t* dungeonPtr) {
    turn_t* turn;
    dungeonPtr->turnsHeapPtr = malloc(sizeof(binheap_t));
    binheap_init(dungeonPtr->turnsHeapPtr, turnCompare, turnDelete);
    for (int i = 0; i < dungeonPtr->monsterCount; i++) {
        turn = malloc(sizeof(turn_t));
        turn->monsterPtr = dungeonPtr->monsterPtrs[i];
        turn->nextTurn = (100 / turn->monsterPtr->speed);
        turn->id = i;
        binheap_insert(dungeonPtr->turnsHeapPtr, (void*) turn);
    }
}

void turnDo(dungeon_t* dungeonPtr) {
    turn_t *turnPtr;
    do {
        turnPtr = binheap_remove_min(dungeonPtr->turnsHeapPtr);
        if (!turnPtr->monsterPtr->alive) {
            free(turnPtr);
            turnPtr = NULL;
        }
    } while (!turnPtr);
    moveMonsterLogic(dungeonPtr, turnPtr->monsterPtr);
    turnPtr->nextTurn += 100 / turnPtr->monsterPtr->speed;
    binheap_insert(dungeonPtr->turnsHeapPtr, (void*) turnPtr);
    mvprintw(0, 0, "Moved %c", monsterGetChar(*turnPtr->monsterPtr));
    refresh();
}

void turnDoPC(dungeon_t* dungeonPtr) {
    int userChar;
    int validChar = 0;
    int dstX;
    int dstY;
    turn_t* turnPtr;
    turnPtr = binheap_remove_min(dungeonPtr->turnsHeapPtr);
    dstX = turnPtr->monsterPtr->x;
    dstY = turnPtr->monsterPtr->y;

    do {
        userChar = wgetch(stdscr);
        switch (userChar) {
            case '7': //Num Lock on
            case KEY_HOME: //Num Lock off
            case 'y':
                mvprintw(0, 0, "User entered up-left");
                dstX--;
                dstY--;
                validChar = 1;
                break;
            case '8': //Num Lock on
            case KEY_UP: //Num Lock off
            case 'k':
            case 'w':
                mvprintw(0, 0, "User entered up");
                dstY--;
                validChar = 1;
                break;
            case '9': //Num Lock on
            case KEY_PPAGE: //Num Lock off
            case 'u':
                mvprintw(0, 0, "User entered up-right");
                dstX++;
                dstY--;
                validChar = 1;
                break;
            case '4': //Num Lock on
            case KEY_LEFT: //Num Lock off
            case 'h':
            case 'a':
                mvprintw(0, 0, "User entered left");
                dstX--;
                validChar = 1;
                break;
            case '6': //Num Lock on
            case KEY_RIGHT: //Num Lock off
            case 'l':
            case 'd':
                mvprintw(0, 0, "User entered right");
                dstX++;
                validChar = 1;
                break;
            case '1': //Num Lock on
            case KEY_END: //Num Lock off
            case 'b':
                mvprintw(0, 0, "User entered down-left");
                dstX--;
                dstY++;
                validChar = 1;
                break;
            case '2': //Num Lock on
            case KEY_DOWN: //Num Lock off
            case 'j':
            case 's':
                mvprintw(0, 0, "User entered down");
                dstY++;
                validChar = 1;
                break;
            case '3': //Num Lock on
            case KEY_NPAGE: //Num Lock off
            case 'n':
                mvprintw(0, 0, "User entered down-right");
                dstX++;
                dstY++;
                validChar = 1;
                break;
            default:
                mvprintw(0, 0, "Other key");
        }
    } while (!validChar);

    moveMonster(dungeonPtr, turnPtr->monsterPtr, dstX, dstY);
    turnPtr->nextTurn += 100 / turnPtr->monsterPtr->speed;
    binheap_insert(dungeonPtr->turnsHeapPtr, (void*) turnPtr);
}

int turnIsPC(dungeon_t* dungeonPtr) {
    turn_t* turnPtr;
    turnPtr = binheap_peek_min(dungeonPtr->turnsHeapPtr);
    return turnPtr->monsterPtr->isPC;
}

void turnDestroy(dungeon_t* dungeonPtr) {
    binheap_delete(dungeonPtr->turnsHeapPtr);
    free(dungeonPtr->turnsHeapPtr);
}

int turnCompare(const void* d1, const void* d2) {
    turn_t* t1 = (turn_t*) d1;
    turn_t* t2 = (turn_t*) d2;
    if (t1->nextTurn == t2->nextTurn) {
        return t1->id - t2->id;
    }
    return t1->nextTurn - t2->nextTurn;
}

void turnDelete(void* d) {
    free(d);
}
