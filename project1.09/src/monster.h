//
// Created by joelm on 2016-02-23.
//

#ifndef PROJECT_MONSTER_H
#define PROJECT_MONSTER_H

#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "dice_set.h"
#include "monster_descrip.h"
#include "item_descrip.h"

#define MONSTER_MIN_SPEED 5
#define MONSTER_MAX_SPEED 20

#define MONSTER_INTELLIGENT 0x8
#define MONSTER_TELEPATHIC 0x4
#define MONSTER_TUNNELING 0x2
#define MONSTER_ERRATIC 0x1

typedef struct gridCell_struct gridCell_t;
typedef struct dungeon_struct dungeon_t;

class monster {
public:
    int x;
    int y;
    bool alive = true;

    std::string name;
    std::string description;
    int color;
    int speed;
    int HP;
    dice_set* DAM_ptr;
    char symb;

public:
    monster(std::string name, std::string desc, int color, int speed, int HP, dice_set* DAM_ptr, char SYMB) {
        this->name = name;
        this->description = desc;
        this->color = color;
        this->speed = speed;
        this->HP = HP;
        this->DAM_ptr = DAM_ptr;
        this->symb = SYMB;

        this->x = 0;
        this->y = 0;
    }
    virtual ~monster() {};
    char* toString(dungeon_t* dungeonPtr);
    // Override getChar
    virtual char getChar();
    virtual bool isPC() = 0;
    int attack(monster& other);
};

class monster_evil : public monster {
public:
    int lastPCX;
    int lastPCY;
    int abilities;

public:
    monster_evil(std::string name, std::string desc, int color, int speed, int abil, int HP, dice_set* DAM_ptr, char SYMB)
            : monster::monster(name, desc, color, speed, HP, DAM_ptr, SYMB) {
        this->lastPCX = 0;
        this->lastPCY = 0;
        this->abilities = abil;
    }
    bool isPC() { return false; }
};

class monster_PC : public monster {
private:
    item* inventory[10];
    item* eqipment[12];
public:
    gridCell_t** gridKnown;

public:
    monster_PC();
    ~monster_PC();
    bool isPC() { return true; }
    void updateGridKnown(gridCell_t** world);
    int show_inventory(bool esc_only);
    int show_equipment(bool esc_only);
    bool pick_up(item& object);
    void drop_item(dungeon_t& dungeon);
    void take_off_item();
};

typedef struct dungeon_struct dungeon_t;

void initMonsters(dungeon_t *dungeonPtr, std::vector<monster_descrip*>& monster_descrips);
void initItems(dungeon_t& dungeon, std::vector<item_descrip*>& item_descrips);
void monstersDestroy(dungeon_t *dungeonPtr);
void itemsDestroy(dungeon_t& dungeon);
void monsterList(dungeon_t *dungeonPtr);

#endif //PROJECT_MONSTER_H
