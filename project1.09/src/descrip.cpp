//
// Created by joelm on 2016-04-05.
//

#include <iostream>
#include <sstream>
#include "descrip.h"
#include "utils.h"

int descrip::parse_color(std::string color_str) {
    for (uint i = 0; i < sizeof(colors); i++) {
        if (color_str == colors[i].name) {
            return colors[i].color;
        }
    }
    std::cout << "Warning: Color not known: " << color_str;
    return COLOR_WHITE;
}

void descrip::read_field(std::ifstream& input, std::string& field, bool &has_field) {
    if (has_field) {
        throw "Duplicate field";
    }
    has_field = true;
    std::getline(input, field);
    util_remove_cr(field);
}

void descrip::read_multiline(std::ifstream& input, std::string &field, bool &has_field) {
    std::string temp;
    if (has_field) {
        throw "Dulpciate multiline field";
    }
    has_field = true;
    std::ostringstream temp_field;
    // finish the keyword line
    std::getline(input, temp);
    // Read the first line of the field contents
    std::getline(input, temp);
    do {
        temp_field << temp << std::endl;
        std::getline(input, temp);
        util_remove_cr(temp);
    } while (temp != ".");
    // remove the trailing '\n'
    field = temp_field.str();
    field.erase(field.size() - 1);
}