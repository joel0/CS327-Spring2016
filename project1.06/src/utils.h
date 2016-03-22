//
// Created by joelm on 2016-02-14.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

#include <stddef.h>
#include "dungeon.h"

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a < b ? b : a

#ifdef __cplusplus
extern "C" {
#endif

int malloc2DArray(void ***grid, size_t size, int width, int height);
void free2DArray(void **grid, int height);
direction_t utilRandDir();

#ifdef __cplusplus
}
#endif

#endif //PROJECT_UTILS_H
