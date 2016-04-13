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
#include "movement.h"
#include "message_queue.h"
#include "screen.h"

int monster::attack(monster& other) {
    int dam = DAM_ptr->roll();
    other.HP -= dam;
    if (other.HP < 0) {
        other.alive = false;
    }
    return dam;
}

char monster::getChar() {
    return symb;
}

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

monster_PC::monster_PC() :
        monster(std::string("PC"), std::string("You"), COLOR_WHITE, 10, 1000, new dice_set(0, 1, 4), '@') {
    malloc2DArray((void***) &gridKnown, sizeof(**gridKnown), WIDTH, HEIGHT);
    for (int curY = 0; curY < HEIGHT; curY++) {
        for (int curX = 0; curX < WIDTH; curX++) {
            gridKnown[curY][curX].hardness = 0;
            gridKnown[curY][curX].material = rock;
            gridKnown[curY][curX].monsterPtr = NULL;
            gridKnown[curY][curX].itemPtr = NULL;
        }
    }
    for (int i = 0; i < 10; i++) {
        inventory[i] = NULL;
    }
    for (int i = 0; i < 12; i++) {
        eqipment[i] = NULL;
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

int monster_PC::show_inventory(bool esc_only) {
    int exit = 0;
    int userChar;
    int offset = 0;
    int maxOffset = 10 + 4 - 11;
    char itemLine[60];
    WINDOW* monsterWin = newpad(10 + 4, 60);
    wbkgd(monsterWin, COLOR_PAIR(COLOR_INVERTED));
    wborder(monsterWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(monsterWin, 1, 60 / 2 - 9 / 2, "INVENTORY");
    wmove(monsterWin, 2, 1);
    whline(monsterWin, ACS_HLINE, 58);
    for (int i = 0; i < 10; i++) {
        if (inventory[i] == NULL) {
            sprintf(itemLine, "%d. Empty", i);
            mvwaddstr(monsterWin, i + 3, 1, itemLine);
        } else {
            sprintf(itemLine, "%d. %s", i, inventory[i]->name.c_str());
            wattron(monsterWin, COLOR_PAIR(inventory[i]->color + 10));
            mvwaddstr(monsterWin, i + 3, 1, itemLine);
            wattroff(monsterWin, COLOR_PAIR(inventory[i]->color + 10));
        }
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
            default:
                if (!esc_only) {
                    exit = 1;
                }
                break;
        }
    } while (!exit);
    delwin(monsterWin);
    refresh();
    return userChar;
}

int monster_PC::show_equipment(bool esc_only) {
    int exit = 0;
    int userChar;
    int offset = 0;
    int maxOffset = 12 + 4 - 11;
    char eqipmentLine[60];
    WINDOW* monsterWin = newpad(12 + 4, 60);
    wbkgd(monsterWin, COLOR_PAIR(COLOR_INVERTED));
    wborder(monsterWin, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(monsterWin, 1, 60 / 2 - 9 / 2, "EQUIPMENT");
    wmove(monsterWin, 2, 1);
    whline(monsterWin, ACS_HLINE, 58);
    for (int i = 0; i < 12; i++) {
        if (eqipment[i] == NULL) {
            sprintf(eqipmentLine, "%c. Empty", i + 'a');
            mvwaddstr(monsterWin, i + 3, 1, eqipmentLine);
        } else {
            sprintf(eqipmentLine, "%c. %s", i + 'a', eqipment[i]->name.c_str());
            wattron(monsterWin, COLOR_PAIR(eqipment[i]->color + 10));
            mvwaddstr(monsterWin, i + 3, 1, eqipmentLine);
            wattroff(monsterWin, COLOR_PAIR(eqipment[i]->color + 10));
        }
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
            default:
                if (!esc_only) {
                    exit = 1;
                }
                break;
        }
    } while (!exit);
    delwin(monsterWin);
    refresh();
    return userChar;
}

bool monster_PC::pick_up(item &object) {
    int i = 0;
    while (i < 10) {
        if (inventory[i] == NULL) {
            inventory[i] = &object;
            return true;
        }
        i++;
    }
    return false;
}

void monster_PC::drop_item(dungeon_t& dungeon) {
    if (dungeon.grid[y][x].itemPtr != NULL) {
        message_queue::instance()->enqueue("I can not drop an item onto another item. Please move to an empty area.");
        return;
    }
    int choice_char = show_inventory(false);
    if (choice_char == 27) {
        // ESC is valid
        return;
    }
    if (choice_char < '0' || choice_char > '9') {
        message_queue::instance()->enqueue("That is not a valid item to drop.");
        return;
    }
    int choice = choice_char - '0';
    if (inventory[choice] == NULL) {
        message_queue::instance()->enqueue("There is no item in that slot to drop.");
        return;
    }
    std::stringstream msg_str;
    msg_str << "You have dropped " << inventory[choice]->name;
    message_queue::instance()->enqueue(msg_str);
    dungeon.grid[y][x].itemPtr = inventory[choice];
    inventory[choice] = NULL;
}

void monster_PC::inspect_item() {
    int choice_char = show_inventory(false);
    if (choice_char == 27) {
        // ESC is valid
        return;
    }
    if (choice_char < '0' || choice_char > '9') {
        message_queue::instance()->enqueue("That is not a valid item to drop.");
        return;
    }
    int choice = choice_char - '0';
    if (inventory[choice] == NULL) {
        message_queue::instance()->enqueue("There is no item in that slot to inspect.");
        return;
    }
    screen_show_dialog(inventory[choice]->name, inventory[choice]->desc);
}

// To keep track of how many monsters to free in the destroy
static int totalMonsters;

void initMonsters(dungeon_t* dungeonPtr, std::vector<monster_descrip*>& monster_descrips) {
    monster* randMonsterPtr;
    dungeonPtr->PCPtr = new monster_PC();
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