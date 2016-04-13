//
// Created by joelm on 2016-03-07.
//

#include <curses.h>

#include "screen.h"
#include "globals.h"

static int count_lines(std::string msg);

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

void screen_show_dialog(std::string title, std::string msg) {
    int height = count_lines(msg) + 2;
    int top = HEIGHT / 2 - height / 2;
    WINDOW* bg_win = newwin(height + 2, 80, top, 0);
    WINDOW* msg_win = newwin(height, 78, top + 1, 1);
    wbkgd(bg_win, COLOR_PAIR(COLOR_INVERTED));
    wbkgd(msg_win, COLOR_PAIR(COLOR_INVERTED));
    wborder(bg_win, 0, 0, 0, 0, 0, 0, 0, 0);
    mvwaddstr(msg_win, 0, 78 / 2 - (int) title.length() / 2, title.c_str());
    wmove(msg_win, 1, 0);
    whline(msg_win, ACS_HLINE, 78);
    mvwaddstr(msg_win, 2, 0, msg.c_str());
    wrefresh(bg_win);
    wrefresh(msg_win);
    doupdate();

    getch();
    wnoutrefresh(stdscr);
    delwin(bg_win);
    delwin(msg_win);
    refresh();
}

static int count_lines(std::string msg) {
    int count = 1;
    for (int i = 0; i < msg.length(); i++) {
        if (msg[i] == '\n') {
            count++;
        }
    }
    return count;
}