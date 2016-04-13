//
// Created by joelm on 2016-04-12.
//

#ifndef PROJECT0_MESSAGE_QUEUE_H
#define PROJECT0_MESSAGE_QUEUE_H

#include <vector>

class message_queue {
private:
    static message_queue* queue_instance;
    std::vector<std::string> queue;

public:
    static message_queue* instance();
    void enqueue(std::string* msg);
    void enqueue(std::stringstream& stream);
    void print_all();
};

#endif //PROJECT0_MESSAGE_QUEUE_H
