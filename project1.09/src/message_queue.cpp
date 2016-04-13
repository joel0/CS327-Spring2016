//
// Created by joelm on 2016-04-12.
//

#include <curses.h>
#include <sstream>

#include "message_queue.h"
#include "screen.h"

void message_queue::enqueue(std::string* msg) {
    queue.push_back(*msg);
}

void message_queue::enqueue(std::stringstream& stream) {
    std::string s = stream.str();
    enqueue(&s);
}

void message_queue::print_all() {
    if (queue.size() > 1) {
        for (int i = 0; i < queue.size(); i++) {
            screenClearRow(0);
            mvprintw(0, 0, "(%d/%d) %s", i + 1, queue.size(), queue[i].c_str());
            getch();
        }
    } else if (queue.size() == 1) {
        screenClearRow(0);
        mvprintw(0, 0, "%s", queue[0].c_str());
        getch();
    }
    screenClearRow(0);
    queue.clear();
}

message_queue* message_queue::queue_instance = NULL;

message_queue *message_queue::instance() {
    if (queue_instance == NULL) {
        queue_instance = new message_queue();
    }
    return queue_instance;
}