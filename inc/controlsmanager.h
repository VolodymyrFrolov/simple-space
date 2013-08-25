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

enum MOUSE_KEY {
    MOUSE_LEFT_KEY,
    MOUSE_MIDDLE_KEY,
    MOUSE_RIGHT_KEY
};

enum MOUSE_KEY_ACTION {
    MOUSE_KEY_DOWN,
    MOUSE_KEY_UP
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

    void update(bool is_pressed, int x, int y);
};

struct Mouse {
    int x;
    int y;

    MouseKey left_key;
    MouseKey middle_key;
    MouseKey right_key;

    Mouse(int x = 0, int y = 0) : x(x), y(y) {}
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
    _is_pressed(false),
    _is_mouse_over(false),
    _label(label),
    _button_callback(button_callback) {}

    bool is_pressed() const {return _is_pressed;}
    bool is_mouse_over() const {return _is_mouse_over;}
    std::string get_label() const {return _label;}
    void set_label(std::string label) {_label = label;}

    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    virtual void draw() const;
};
 
class ControlsManager {
    std::vector<Button> buttons;
    bool mouse_over_button(const Button& button, const int& mouse_x, const int& mouse_y) const;
public:
    int add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback);
    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    void draw_buttons() const;

    int find_id_by_label(std::string label) const;
    void set_label_by_id(std::string label, int id);
};

#endif /* defined(__controls_manager__simplespace__) */

