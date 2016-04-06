//
// Created by joelm on 2016-04-05.
//

#ifndef PROJECT0_MONSTER_DESCRIP_H
#define PROJECT0_MONSTER_DESCRIP_H

#include <fstream>
#include "dice_set.h"
#include "monster.h"

class monster_descrip {
public:
    std::string name;
    std::string description;
    int color;
    dice_set* speed_ptr;
    std::string abilities;
    dice_set* HP_ptr;
    dice_set* DAM_ptr;
    char symb = '-';

public:
    monster_descrip(std::ifstream &input);
    ~monster_descrip();
    std::string to_string();
    monster_evil* generate();
private:
    int parse_color(std::string color_str);
};

#endif //PROJECT0_MONSTER_DESCRIP_H
