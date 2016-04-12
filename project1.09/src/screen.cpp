//
// Created by joelm on 2016-03-07.
//

#include <curses.h>

#include "screen.h"
#include "globals.h"

void initTerminal() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();
    init_pair(COLOR_INVERTED, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_FAKE_BLACK, COLOR_WHITE, COLOR_BLACK);

    // On a white BG for the monsters list
    init_pair(COLOR_RED + 10, COLOR_RED, COLOR_WHITE);
    init_pair(COLOR_GREEN + 10, COLOR_GREEN, COLOR_WHITE);
    init_pair(COLOR_BLUE + 10, COLOR_BLUE, COLOR_WHITE);
    init_pair(COLOR_CYAN + 10, COLOR_CYAN, COLOR_WHITE);
    init_pair(COLOR_YELLOW + 10, COLOR_YELLOW, COLOR_WHITE);
    init_pair(COLOR_MAGENTA + 10, COLOR_MAGENTA, COLOR_WHITE);
    init_pair(COLOR_WHITE + 10, COLOR_BLACK, COLOR_WHITE);
    init_pair(COLOR_FAKE_BLACK + 10, COLOR_BLACK, COLOR_WHITE);
}

void screenClearRow(int row) {
    move(row, 0);
    clrtoeol();
}