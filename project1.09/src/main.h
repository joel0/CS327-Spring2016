//
// Created by joelm on 2016-01-25.
//

#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H

#include "descrip.h"

void showUsage(char* name);
char* dungeonFileName();
template<typename T> void delete_descrips(std::vector<T*>& descrips);

#endif //PROJECT_MAIN_H