//
// Created by joelm on 2016-03-29.
//

#ifndef PROJECT0_DICE_SET_H
#define PROJECT0_DICE_SET_H

#include <string>

class dice_set {
public:
    int base;
    int dice;
    int sides;

public:
    dice_set() {}
    dice_set(int base, int dice, int sides) {
        this->base = base;
        this->dice = dice;
        this->sides = sides;
    }
    dice_set(const char* dice_str);
    int roll();
    std::string toString();
};

#endif //PROJECT0_DICE_SET_H
