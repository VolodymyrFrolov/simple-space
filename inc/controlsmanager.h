//
//  controlsmanager.h
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __controls_manager__simplespace__
#define __controls_manager__simplespace__

#include <iostream>
#include <vector>
using std::cout;   // temp
using std::endl;   // temp

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <GLUT/glut.h>
#elif __linux__
  //#include <GL/glut.h>
    #include <GL/freeglut.h>
#else
    // Unsupproted platform
#endif

struct Mouse {
    int x;
    int y;

    bool left_key_down;
    int left_key_down_x;
    int left_key_down_y;

    bool right_key_down;
    int right_key_down_x;
    int right_key_down_y;
};

typedef void (*ButtonCallback)();

struct Button {
    unsigned int id;
    int x;
    int y;
    int width;
    int height;
    bool is_pressed;
    bool is_mouse_over;
    std::string label;
    ButtonCallback button_callback;

    // Define operator < to use Button in std::set
    bool operator< (const Button & rhs) const {
        return id < rhs.id;
    }
 };
 
class ControlsManager {

    const unsigned int buttons_number_max; // std::numeric_limits<unsigned int>::max()
    std::vector<Button> buttons;

    void draw_label(std::string label, int x, int y, void* font) const;

    public:
    ControlsManager();
    void add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback);
    void update_controls(const Mouse& mouse);
    void draw_buttons() const;

};

#endif /* defined(__controls_manager__simplespace__) */

