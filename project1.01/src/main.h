//
// Created by joelm on 2016-01-25.
//

#ifndef PROJECT_MAIN_H
#define PROJECT_MAIN_H

#define WIDTH 80
#define HEIGHT 21
#define ROOM_WIDTH_MIN 3
#define ROOM_WIDTH_MAX 20
#define ROOM_HEIGHT_MIN 2
#define ROOM_HEIGHT_MAX 10

enum material_enum {
    room1 = '.',
    corridor = '#',
    rock = ' '
};

typedef struct room_struct {
    int x;
    int y;
    int height;
    int width;
} room_t;

typedef struct gridCell_struct {
    int hardness;
    enum material_enum material;
} gridCell_t;

typedef struct point_struct {
    int x;
    int y;
} point_t;

#endif //PROJECT_MAIN_H