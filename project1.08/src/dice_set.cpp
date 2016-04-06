//
// Created by joelm on 2016-03-29.
//

#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include "dice_set.h"

dice_set::dice_set(const char *dice_str) {
    char* base_str;
    char* count_str;
    size_t base_len = 0;
    size_t count_len = 0;
    // Base
    do {
        if (dice_str[base_len++] == '\0') {
            return;
        }
    } while (dice_str[base_len] != '+');
    base_str = (char*) malloc(sizeof(char) * (base_len + 1));
    strncpy(base_str, dice_str, base_len);
    base_str[base_len] = '\0';
    base = atoi(base_str);
    free(base_str);

    // Dice
    do {
        if (dice_str[1 + base_len + count_len++] == '\0') {
            return;
        }
    } while (dice_str[1 + base_len + count_len] != 'd');
    count_str = (char*) malloc(sizeof(char) * (count_len + 1));
    strncpy(count_str, dice_str + base_len + 1, sizeof(char) * (count_len + 1));
    count_str[count_len] = '\0';
    dice = atoi(count_str);
    free(count_str);

    // Sides
    sides = atoi(dice_str + base_len + count_len + 2);
}

int dice_set::roll() {
    int total = base;
    for (int i = 0; i < dice; i++) {
        total += (rand() % sides) + 1;
    }
    return total;
}

std::string dice_set::toString() {
    std::ostringstream out;
    out << base << "+" << dice << "d" << sides;
    return out.str();
}

