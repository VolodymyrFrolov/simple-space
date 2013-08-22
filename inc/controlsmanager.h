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

class UIControl {
protected:
    int _id, _x, _y, _w, _h;
public:
    UIControl(int id, int x, int y, int w, int h) :
    _id(id), _x(x), _y(y), _w(w), _h(h) {}
    int get_id() const {return _id;}
    virtual void draw() const = 0;
};

class Button : public UIControl {

    bool _is_pressed;
    bool _is_mouse_over;
    std::string _label;
    ButtonCallback _button_callback;

    bool mouse_over_button(const int& mouse_x, const int& mouse_y) const;

public:

    Button(int id, int x, int y, int w, int h,
            std::string label,
            ButtonCallback button_callback) :
    UIControl(id, x, y, w, h),
    _label(label),
    _button_callback(button_callback),
    _is_pressed(false),
    _is_mouse_over(false) {}

    bool is_pressed() const {return _is_pressed;}
    bool is_mouse_over() const {return _is_mouse_over;}
    std::string get_label() const {return _label;}
    void set_label(std::string label) {_label = label;}

    void handle_mouse_move(const Mouse& mouse);
    void handle_button_down(const Mouse& mouse);
    void handle_button_up(const Mouse& mouse);
    virtual void draw() const;
};
 
class ControlsManager {
    std::vector<Button> buttons;
    bool mouse_over_button(const Button& button, const int& mouse_x, const int& mouse_y) const;
public:
    void add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback);
    void handle_mouse_move(const Mouse& mouse);
    void handle_button_down(const Mouse& mouse);
    void handle_button_up(const Mouse& mouse);
    void draw_buttons() const;
};

#endif /* defined(__controls_manager__simplespace__) */

