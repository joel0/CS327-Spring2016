//
// Created by joelm on 2016-02-14.
//

#include <stdlib.h>
#include <cstring>
#include "utils.h"

int malloc2DArray(void*** grid, size_t size, int width, int height) {
    if (!(*grid = (void **) malloc(height * sizeof(void*)))) {
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

void util_remove_cr(std::string& str) {
    // This code is to remove the \r from the end of strings to handle Windows \r\n
    // This code from http://stackoverflow.com/a/2529011
    if (!str.empty() && str[str.size() - 1] == '\r')
        str.erase(str.size() - 1);
}