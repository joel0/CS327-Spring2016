//
// Created by joelm on 2016-04-06.
//

#ifndef PROJECT0_ITEM_H
#define PROJECT0_ITEM_H

#include <string>
#include "dice_set.h"

class item {
public:
    std::string name;
    std::string desc;
    int type;
    int color;
    int hit_bonus;
    dice_set* dam_bonus_ptr;
    int dodge_bonus;
    int def_bonus;
    int weight;
    int speed_bonus;
    int special_attrib;
    int value;
    char symb;

public:
    item(std::string name, std::string desc, int type, int color, int hit_bonus, dice_set* dam_bonus_ptr,
            int dodge_bonus, int def_bonus, int weight, int speed_bonus, int special_attrib, int value, char symb) {
        this->name = name;
        this->desc = desc;
        this->type = type;
        this->color = color;
        this->hit_bonus = hit_bonus;
        this->dam_bonus_ptr = dam_bonus_ptr;
        this->dodge_bonus = dodge_bonus;
        this->def_bonus = def_bonus;
        this->weight = weight;
        this->speed_bonus = speed_bonus;
        this->special_attrib = special_attrib;
        this->value = value;
        this->symb = symb;
    }
    bool is_equipment();
};

#endif //PROJECT0_ITEM_H
