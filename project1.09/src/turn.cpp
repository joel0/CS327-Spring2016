//
// Created by joelm on 2016-02-23.
//

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <sstream>

#include "turn.h"
#include "movement.h"
#include "screen.h"
#include "path.h"
#include "message_queue.h"

int turnCompare(const void* d1, const void* d2);
void turnDelete(void* d);

void turnInit(dungeon_t* dungeonPtr) {
    turn_t* turn;
    dungeonPtr->turnsHeapPtr = (heap_t *) malloc(sizeof(heap_t));
    heap_init(dungeonPtr->turnsHeapPtr, turnCompare, turnDelete);
    for (int i = 0; i < dungeonPtr->monsterCount; i++) {
        turn = (turn_t *) malloc(sizeof(turn_t));
        turn->monsterPtr = dungeonPtr->monsterPtrs[i];
        turn->nextTurn = (100 / turn->monsterPtr->speed);
        turn->id = i;
        heap_insert(dungeonPtr->turnsHeapPtr, (void*) turn);
    }
}

void turnDo(dungeon_t* dungeonPtr) {
    turn_t *turnPtr;
    do {
        turnPtr = (turn_t *) heap_remove_min(dungeonPtr->turnsHeapPtr);
        if (!turnPtr->monsterPtr->alive) {
            free(turnPtr);
            turnPtr = NULL;
        }
    } while (!turnPtr);
    moveMonsterLogic(dungeonPtr, turnPtr->monsterPtr);
    turnPtr->nextTurn += 100 / turnPtr->monsterPtr->speed;
    heap_insert(dungeonPtr->turnsHeapPtr, (void*) turnPtr);

//    screenClearRow(0);
//    mvprintw(0, 0, "Moved %c", monsterGetChar(*turnPtr->monsterPtr));
//    refresh();
}

PC_action turnDoPC(dungeon_t* dungeonPtr) {
    PC_action returnValue = actionMovement;
    int userChar;
    int validChar = 0;
    int dstX;
    int dstY;
    turn_t* turnPtr;
    turnPtr = (turn_t *) heap_remove_min(dungeonPtr->turnsHeapPtr);
    dstX = turnPtr->monsterPtr->x;
    dstY = turnPtr->monsterPtr->y;

    // Print any messages
    message_queue::instance()->print_all();
    mvprintw(0, 0, "Your HP: %d", dungeonPtr->PCPtr->HP);

    do {
        userChar = getch();
        screenClearRow(0);
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
            case ' ':
            case '5': //Num Lock on
            case 350: //Num Lock off (from testing, I can't find a #define)
                //do nothing
                validChar = 1;
                break;
            case '>':
                if (dungeonPtr->grid[dstY][dstX].material == stairsDn) {
                    returnValue = actionStairsDn;
                    mvprintw(0, 0, "Going down the stairs.");
                    refresh();
                    validChar = 1;
                } else {
                    mvprintw(0, 0, "I can't dig a hole through the floor. Please find me some stairs to use.");
                }
                break;
            case '<':
                if (dungeonPtr->grid[dstY][dstX].material == stairsUp) {
                    returnValue = actionStairsUp;
                    mvprintw(0, 0, "Going up the stairs.");
                    refresh();
                    validChar = 1;
                } else {
                    mvprintw(0, 0, "I can't reach the ceiling from here. Please find me some stairs to use.");
                }
                break;
            case 'm':
                returnValue = actionListMonsters;
                validChar = 1;
                break;
            case 'i':
                returnValue = actionListInventory;
                validChar = 1;
                break;
            case 'e':
                returnValue = actionListEquipment;
                validChar = 1;
                break;
            case 's':
                returnValue = actionSave;
                validChar = 1;
                break;
            default:
                mvprintw(0, 0, "You can't do that! (%d)", userChar);
        }
    } while (!validChar);

    if (returnValue == actionMovement) {
        moveMonster(dungeonPtr, turnPtr->monsterPtr, dstX, dstY);
        if (turnPtr->monsterPtr->isPC() && turnPtr->monsterPtr->x == dstX && turnPtr->monsterPtr->y == dstY) {
            // The PC has just moved into the desired location
            if (dungeonPtr->grid[dstY][dstX].itemPtr != NULL) {
                // Try to pick up the item
                std::stringstream msg_str;
                if (dungeonPtr->PCPtr->pick_up(*dungeonPtr->grid[dstY][dstX].itemPtr)) {
                    msg_str << "You picked up " << dungeonPtr->grid[dstY][dstX].itemPtr->name;
                    dungeonPtr->grid[dstY][dstX].itemPtr = NULL;
                } else {
                    msg_str << "You have no room to pick up " << dungeonPtr->grid[dstY][dstX].itemPtr->name;
                }
                message_queue::instance()->enqueue(msg_str);
            }
        }
        pathTunneling(dungeonPtr);
        pathNontunneling(dungeonPtr);
        turnPtr->nextTurn += 100 / turnPtr->monsterPtr->speed;
    }
    heap_insert(dungeonPtr->turnsHeapPtr, (void *) turnPtr);
    return returnValue;
}

int turnIsPC(dungeon_t* dungeonPtr) {
    turn_t* turnPtr;
    turnPtr = (turn_t *) heap_peek_min(dungeonPtr->turnsHeapPtr);
    return turnPtr->monsterPtr->isPC();
}

void turnDestroy(dungeon_t* dungeonPtr) {
    heap_delete(dungeonPtr->turnsHeapPtr);
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
