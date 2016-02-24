//
// Created by joelm on 2016-02-14.
//

#include <stdlib.h>
#include "utils.h"
#include "dungeon.h"
#include "stdlib.h"

int malloc2DArray(void*** grid, size_t size, int width, int height) {
    if (!(*grid = malloc(height * sizeof(void*)))) {
        return -1;
    }

    for (int y = 0; y < height; y++) {
        if (!((*grid)[y] = malloc(width * size))) {
            return -1;
        }
    }
    return 0;
}

void free2DArray(void** grid, int height) {
    for (int y = 0; y < height; y++) {
        free(grid[y]);
    }
    free(grid);
}

direction_t utilRandDir() {
    switch (rand() % 8) {
        case 0:
            return northwest;
        case 1:
            return north;
        case 2:
            return northeast;
        case 3:
            return east;
        case 4:
            return southeast;
        case 5:
            return south;
        case 6:
            return southwest;
        case 7:
            return west;
        default:
            // To avoid a compiler warning, something must be returned.
            return north;
    }
}