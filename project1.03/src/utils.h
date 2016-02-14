//
// Created by joelm on 2016-02-14.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

#include <stddef.h>

int malloc2DArray(void*** grid, size_t size, int width, int height);
void free2DArray(void** grid, int height);

#endif //PROJECT_UTILS_H
