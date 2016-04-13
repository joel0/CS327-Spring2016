//
// Created by joelm on 2016-03-07.
//

#ifndef PROJECT_SCREEN_H
#define PROJECT_SCREEN_H

#include <string>

void initTerminal();
void screenClearRow(int row);
void screen_show_dialog(std::string title, std::string msg);

#endif //PROJECT_SCREEN_H
