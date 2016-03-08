//
// Created by joelm on 2016-03-07.
//

#include <curses.h>

#include "screen.h"

void initTerminal() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
}

void screenClearRow(int row) {
    move(row, 0);
    clrtoeol();
}