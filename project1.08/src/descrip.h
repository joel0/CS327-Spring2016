//
// Created by joelm on 2016-04-05.
//

#ifndef PROJECT0_DESCRIP_H
#define PROJECT0_DESCRIP_H

#include <string>
#include <curses.h>
#include <fstream>
#include "globals.h"

typedef struct {
    const char* name;
    int color;
} color_t;

const color_t colors[] = {
        {"BLACK", COLOR_FAKE_BLACK},
        {"RED", COLOR_RED},
        {"GREEN", COLOR_GREEN},
        {"YELLOW", COLOR_YELLOW},
        {"BLUE", COLOR_BLUE},
        {"MAGENTA", COLOR_MAGENTA},
        {"CYAN", COLOR_CYAN},
        {"WHITE", COLOR_WHITE},
        {"", COLOR_FAKE_BLACK} };

class descrip {
protected:
    static int parse_color(std::string color_str);
    static void read_field(std::ifstream& input, std::string& field, bool& has_field);
    static void read_multiline(std::ifstream& input, std::string& field, bool& has_field);
};

#endif //PROJECT0_DESCRIP_H
