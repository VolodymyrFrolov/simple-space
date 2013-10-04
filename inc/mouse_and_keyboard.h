//
//  mouse.h
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __mouse__simplespace__
#define __mouse__simplespace__

#include <iostream>

enum KEY_ACTION {
    KEY_DOWN,
    KEY_UP
};

enum ARROW_KEY {
    ARROW_LEFT,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN
};

enum MOUSE_KEY {
    MOUSE_LEFT_KEY,
    MOUSE_MIDDLE_KEY,
    MOUSE_RIGHT_KEY
};

struct MouseKey {
    bool is_down;
    int down_x;
    int down_y;

    MouseKey(bool is_down = false,
             int down_x = 0,
             int down_y = 0) :
    is_down(is_down),
    down_x(down_x),
    down_y(down_y) {}

    void update(bool is_pressed, int x, int y) {
        is_down = is_pressed;
        if (is_pressed) {
            down_x = x;
            down_y = y;
        }
    };
};

struct Mouse {
    int x;
    int y;

    MouseKey left_key;
    MouseKey middle_key;
    MouseKey right_key;

    Mouse(int x = 0, int y = 0) : x(x), y(y) {}
};

#endif /* defined(__mouse__simplespace__) */
