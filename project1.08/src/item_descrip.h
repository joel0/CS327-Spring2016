//
// Created by joelm on 2016-04-05.
//

#ifndef PROJECT0_ITEM_DESCRIP_H
#define PROJECT0_ITEM_DESCRIP_H

#include <string>
#include "dice_set.h"
#include "descrip.h"
#include "item.h"

class item_descrip : private descrip {
public:
    std::string name;
    std::string description;
    int type;
    int color;
    dice_set* hit_bonus_ptr;
    dice_set* dam_bonus_ptr;
    dice_set* dodge_bonus_ptr;
    dice_set* def_bonus_ptr;
    dice_set* weight_ptr;
    dice_set* speed_bonus_ptr;
    dice_set* special_attrib_ptr;
    dice_set* value_ptr;

public:
    item_descrip(std::ifstream& input);
    ~item_descrip();
    bool is_equipment();
    std::string to_string();
    char get_char();
    item* generate();
private:
    int parse_type(std::string type_str);
};

#endif //PROJECT0_ITEM_DESCRIP_H
