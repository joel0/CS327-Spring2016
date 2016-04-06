//
// Created by joelm on 2016-04-05.
//

#include <sstream>
#include "item_descrip.h"
#include "utils.h"

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
    TYPE_CONTAINER  = 0x40000
} type_enum;

typedef struct {
    const char* name;
    type_enum value;
} type_t;

static type_t types_table[] = {
        {"WEAPON", TYPE_WEAPON},
        {"OFFHAND", TYPE_OFFHAND},
        {"RANGED", TYPE_RANGED},
        {"ARMOR", TYPE_ARMOR},
        {"HELMET", TYPE_HELMET},
        {"CLOAK", TYPE_CLOAK},
        {"GLOVES", TYPE_GLOVES},
        {"BOOTS", TYPE_BOOTS},
        {"RING", TYPE_RING},
        {"AMULET", TYPE_AMULET},
        {"LIGHT", TYPE_LIGHT},
        {"SCROLL", TYPE_SCROLL},
        {"BOOK", TYPE_BOOK},
        {"FLASK", TYPE_FLASK},
        {"GOLD", TYPE_GOLD},
        {"AMMUNITION", TYPE_AMMUNITION},
        {"FOOD", TYPE_FOOD},
        {"WAND", TYPE_WAND},
        {"CONTAINER", TYPE_CONTAINER} };

item_descrip::item_descrip(std::ifstream &input) {
    std::string s;
    std::string keyword;
    bool has_name = false;
    bool has_desc = false;
    bool has_type = false; std::string type_str;
    bool has_color = false; std::string color_str;
    bool has_hit = false; std::string hit_str;
    bool has_dam = false; std::string dam_str;
    bool has_dodge = false; std::string dodge_str;
    bool has_def = false; std::string def_str;
    bool has_weight = false; std::string weight_str;
    bool has_speed = false; std::string speed_str;
    bool has_attr = false; std::string attr_str;
    bool has_val = false; std::string val_str;

    std::getline(input, s);
    util_remove_cr(s);
    while (s != "BEGIN OBJECT") {
        if (s != "") {
            throw std::string("Invalid input outside of item segment: ") + s;
        }
        std::getline(input, s);
        util_remove_cr(s);
        if (input.eof()) { throw std::string("EOF"); }
    }

    while (keyword != "END") {
        if (input.eof()) { throw "Unexpected EOF"; }
        input >> keyword;
        // Clear whitespace to the value
        while (input.peek() == ' ') {
            input.get();
        }
        util_remove_cr(keyword);

        if (keyword == "NAME") {
            read_field(input, name, has_name);
        } else if (keyword == "DESC") {
            read_multiline(input, description, has_desc);
        } else if (keyword == "TYPE") {
            read_field(input, type_str, has_type);
        } else if (keyword == "COLOR") {
            read_field(input, color_str, has_color);
        } else if (keyword == "HIT") {
            read_field(input, hit_str, has_hit);
        } else if (keyword == "DAM") {
            read_field(input, dam_str, has_dam);
        } else if (keyword == "DODGE") {
            read_field(input, dodge_str, has_dodge);
        } else if (keyword == "DEF") {
            read_field(input, def_str, has_def);
        } else if (keyword == "WEIGHT") {
            read_field(input, weight_str, has_weight);
        } else if (keyword == "SPEED") {
            read_field(input, speed_str, has_speed);
        } else if (keyword == "ATTR") {
            read_field(input, attr_str, has_attr);
        } else if (keyword == "VAL") {
            read_field(input, val_str, has_val);
        } else if (keyword == "END") {
            // Do nothing.  Loop will be exited.
        } else {
            std::string temp;
            std::getline(input, temp);
            throw std::string("unknown keyword: ") + keyword;
        }
    }

    if (!has_name || !has_desc || !has_type || !has_color || !has_hit || !has_dam || !has_dodge ||
            !has_def || !has_weight || !has_speed || !has_attr || !has_val) {
        if (has_name) {
            throw std::string("Missing attribute on ") + name;
        } else {
            throw "Missing attribute on monster with no name.";
        }
    }

    color = parse_color(color_str);
    hit_bonus_ptr = new dice_set(hit_str.c_str());
    dam_bonus_ptr = new dice_set(dam_str.c_str());
    dodge_bonus_ptr = new dice_set(dodge_str.c_str());
    def_bonus_ptr = new dice_set(def_str.c_str());
    weight_ptr = new dice_set(weight_str.c_str());
    speed_bonus_ptr = new dice_set(speed_str.c_str());
    special_attrib_ptr = new dice_set(attr_str.c_str());
    value_ptr = new dice_set(val_str.c_str());
    type = parse_type(type_str);
}

item_descrip::~item_descrip() {
    delete hit_bonus_ptr;
    delete dam_bonus_ptr;
    delete dodge_bonus_ptr;
    delete def_bonus_ptr;
    delete weight_ptr;
    delete speed_bonus_ptr;
    delete special_attrib_ptr;
    delete value_ptr;
}


int item_descrip::parse_type(std::string type_str) {
    int out_val = 0;
    bool found;
    std::stringstream type_strs(type_str);
    std::string current_type;

    while (!type_strs.eof()) {
        type_strs >> current_type;
        found = false;
        for (uint i = 0; i < sizeof(types_table); i++) {
            if (current_type == types_table[i].name) {
                out_val |= types_table[i].value;
                found = true;
                break;
            }
        }
        if (!found) {
            std::cout << "Warning: Unknown item type: " << current_type << std::endl;
        }
    }
    return out_val;
}

bool item_descrip::is_equipment() {
    return type < TYPE_SCROLL;
}

std::string item_descrip::to_string() {
    std::stringstream out;
    out << "Name: " << name << std::endl;
    out << "Desc:" << std::endl << description << std::endl;
    return out.str();
}



