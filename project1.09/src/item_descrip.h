//
// Created by joelm on 2016-04-05.
//

#ifndef PROJECT0_ITEM_DESCRIP_H
#define PROJECT0_ITEM_DESCRIP_H

#include <string>
#include "dice_set.h"
#include "descrip.h"
#include "item.h"

typedef enum {
    TYPE_WEAPON     = 0x00001,
    TYPE_OFFHAND    = 0x00002,
    TYPE_RANGED     = 0x00004,
    TYPE_ARMOR      = 0x00008,
    TYPE_HELMET     = 0x00010,
    TYPE_CLOAK      = 0x00020,
    TYPE_GLOVES     = 0x00040,
    TYPE_BOOTS      = 0x00080,
    TYPE_RING       = 0x00100,
    TYPE_AMULET     = 0x00200,
    TYPE_LIGHT      = 0x00400,
    TYPE_SCROLL     = 0x00800,
    TYPE_BOOK       = 0x01000,
    TYPE_FLASK      = 0x02000,
    TYPE_GOLD       = 0x04000,
    TYPE_AMMUNITION = 0x08000,
    TYPE_FOOD       = 0x10000,
    TYPE_WAND       = 0x20000,
    TYPE_CONTAINER  = 0x40000,
    TYPE_STACK      = 0x80000
} item_type_enum;

class item_descrip : public descrip {
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
