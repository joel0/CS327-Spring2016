//
// Created by joelm on 2016-02-14.
//

#include <stdlib.h>
#include "utils.h"

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