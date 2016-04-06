//
// Created by joelm on 2016-04-05.
//

#include <sstream>
#include <curses.h>
#include "monster_descrip.h"
#include "utils.h"

typedef struct {
    const char* name;
    int color;
} color_t;

color_t colors[] = {
                {"BLACK", COLOR_BLACK},
                {"RED", COLOR_RED},
                {"GREEN", COLOR_GREEN},
                {"YELLOW", COLOR_YELLOW},
                {"BLUE", COLOR_BLUE},
                {"MAGENTA", COLOR_MAGENTA},
                {"CYAN", COLOR_CYAN},
                {"WHITE", COLOR_WHITE} };

monster_descrip::monster_descrip(std::ifstream &input) {
    std::string s;
    std::string keyword;
    bool has_name = false;
    bool has_desc = false;
    bool has_color = false; std::string color_str;
    bool has_speed = false; std::string speed_str;
    bool has_abil = false;
    bool has_hp = false; std::string hp_str;
    bool has_dam = false; std::string dam_str;
    bool has_symb = false;

    std::getline(input, s);
    util_remove_cr(s);
    while (s != "BEGIN MONSTER") {
        if (s != "") {
            throw std::string("Invalid input outside of monster segment: ") + s;
        }
        std::getline(input, s);
        util_remove_cr(s);
        if (input.eof()) { throw "EOF"; }
    }

    while (keyword != "END") {
        if (input.eof()) { throw "Unexpected EOF"; }
        input >> keyword;
        // Clear whitespace to the value
        while (input.peek() == ' ') {
            input.get();
        }
        util_remove_cr(keyword);

        //std::cout << "keyword: " << keyword << std::endl;
        if (keyword == "NAME") {
            if (has_name) {
                std::cout << "Warning: duplicate name field";
                throw "Duplicate name field";
            }
            has_name = true;
            std::getline(input, name);
            util_remove_cr(name);
        } else if (keyword == "DESC") {
            std::string temp;
            if (has_desc) {
                throw "Duplicate desc field";
            }
            std::ostringstream temp_desc;
            has_desc = true;
            // finish the DESC line
            std::getline(input, temp);
            // Read the first line of the DESC contents
            std::getline(input, temp);
            do {
                temp_desc << temp << std::endl;
                std::getline(input, temp);
                util_remove_cr(temp);
            } while (temp != ".");
            // remove the trailing '\n'
            description = temp_desc.str();
            description.erase(description.size() - 1);
        } else if (keyword == "COLOR") {
            if (has_color) {
                throw "Duplicate color field";
            }
            has_color = true;
            std::getline(input, color_str);
            util_remove_cr(color_str);
        } else if (keyword == "SPEED") {
            if (has_speed) {
                throw "Duplicate speed field";
            }
            has_speed = true;
            std::getline(input, speed_str);
            util_remove_cr(speed_str);
        } else if (keyword == "ABIL") {
            if (has_abil) {
                throw "Duplicate abil field";
            }
            has_abil = true;
            std::getline(input, abilities);
            util_remove_cr(abilities);
        } else if (keyword == "HP") {
            if (has_hp) {
                throw "Duplicate hp field";
            }
            has_hp = true;
            std::getline(input, hp_str);
            util_remove_cr(hp_str);
        } else if (keyword == "DAM") {
            if (has_dam) {
                throw "Duplicate dam field";
            }
            has_dam = true;
            std::getline(input, dam_str);
            util_remove_cr(dam_str);
        } else if (keyword == "SYMB") {
            if (has_symb) {
                throw "Duplicate symb field";
            }
            std::string temp;
            has_symb = true;
            symb = (char) input.get();
            std::getline(input, temp);
        } else if (keyword == "END") {
            // Do nothing.  Loop wil be exited.
        } else {
            std::string temp;
            std::getline(input, temp);
            throw std::string("unknown keyword: ") + keyword;
        }
    }

    if (!has_name || !has_desc || !has_color || !has_speed || !has_abil || !has_hp || !has_dam || !has_symb) {
        if (has_name) {
            throw std::string("Missing attribute on ") + name;
        } else {
            throw "Missing attribute on monster with no name.";
        }
    }

    color = parse_color(color_str);
    speed_ptr = new dice_set(speed_str.c_str());
    HP_ptr = new dice_set(hp_str.c_str());
    DAM_ptr = new dice_set(dam_str.c_str());
}

monster_descrip::~monster_descrip() {
    delete speed_ptr;
    delete HP_ptr;
    delete DAM_ptr;
}

std::string monster_descrip::to_string() {
    std::stringstream out;
    std::cout << "Name: " << name << std::endl;
    std::cout << "Description: " << std::endl << description << std::endl;
    std::cout << "Color: " << colors[color].name << std::endl;
    std::cout << "Speed: " << speed_ptr->toString() << std::endl;
    std::cout << "Abilities: " << abilities << std::endl;
    std::cout << "Hitpoints: " << HP_ptr->toString() << std::endl;
    std::cout << "Attack Damage: " << DAM_ptr->toString() << std::endl;
    return out.str();
}

monster_evil* monster_descrip::generate() {
    return new monster_evil(name, description, color, speed_ptr, abilities, HP_ptr->roll(), DAM_ptr->roll(), symb);
}

int monster_descrip::parse_color(std::string color_str) {
    for (int i = 0; i < sizeof(colors); i++) {
        if (color_str == colors[i].name) {
            return colors[i].color;
        }
    }
    std::cout << "Warning: Color not known: " << color_str;
    return COLOR_WHITE;
}





