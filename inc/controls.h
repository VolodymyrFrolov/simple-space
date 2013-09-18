//
//  controls.h
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __controls__simplespace__
#define __controls__simplespace__

#include <iostream>
#include <vector>
using std::cout;   // temp
using std::endl;   // temp

#include "timer.h"

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <GLUT/glut.h>
#elif __linux__
  //#include <GL/glut.h>
    #include <GL/freeglut.h>
#else
    // Unsupproted platform
#endif

enum KEY_ACTION {
    KEY_DOWN,
    KEY_UP
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

typedef void (*ActionCallback)();


class UIControl {
protected:
    int _id, _x, _y, _w, _h;
    bool mouse_over_control(const int& mouse_x, const int& mouse_y) const;
public:
    UIControl(int id, int x, int y, int w, int h) :
        _id(id), _x(x), _y(y), _w(w), _h(h) {}
    virtual ~UIControl() {};

    int get_id() const {return _id;}
    std::pair<int, int> get_position() const {return std::make_pair(_x, _y);}

    virtual void handle_mouse_move(const Mouse& mouse) {};
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {};
    virtual void handle_keyboard_key_event(char key, KEY_ACTION action) {};
    virtual void draw() const = 0;
};


class Button : public UIControl {
protected:
    bool _is_pressed;
    bool _is_mouse_over;
    std::string _label;
    ActionCallback _button_callback;

public:

    Button(int id,
           int x, int y,
           int w, int h,
           std::string label,
           ActionCallback button_callback) :
    UIControl(id, x, y, w, h),
    _is_pressed(false),
    _is_mouse_over(false),
    _label(label),
    _button_callback(button_callback) {}

    bool is_pressed() const {return _is_pressed;}
    bool is_mouse_over() const {return _is_mouse_over;}
    std::string get_label() const {return _label;}
    void set_label(std::string label) {_label = label;}

    virtual void handle_mouse_move(const Mouse& mouse);
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void draw() const;
};


class ButtonBoolean : public Button {
    bool _state_on;
    ActionCallback _button_callback_off;

public:
    ButtonBoolean(int id,
                int x, int y,
                int w, int h,
                std::string label,
                bool initial_state,
                ActionCallback action_on,
                ActionCallback action_off) :
    Button(id, x, y, w, h, label, action_on),
    _state_on(initial_state),
    _button_callback_off(action_off) {}
    
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void draw() const;
};


class TextBox : public UIControl {
protected:
    bool _is_active;
    std::string _label;

    Timer _cursor_timer;
    bool _cursor_visible;

    void cursor_toggle() { _cursor_visible = !_cursor_visible; };
    static void static_wrapper_cursor_toggle(void* param) {
        TextBox* pTextBox = (TextBox*)param;
        pTextBox->cursor_toggle();
    }

public:
    TextBox(int id,
            int x, int y,
            int w, int h,
            std::string label) :
    UIControl(id, x, y, w, h),
    _is_active(false),
    _label(label),
    _cursor_timer(650, &TextBox::static_wrapper_cursor_toggle, this, false),
    _cursor_visible(false) {}

    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void handle_keyboard_key_event(char key, KEY_ACTION action);
    virtual void draw() const;
};

class NumericBox : public TextBox {
public:
    NumericBox(int id,
               int x, int y,
               int w, int h,
               std::string label) :
    TextBox(id, x, y, w, h, label) {}

    virtual void handle_keyboard_key_event(char key, KEY_ACTION action);
};

class Slider : public UIControl {
    bool _is_pressed;
    const int _margin;

    double _value;
    double _value_min;
    double _value_max;

    int _slider_pos;
    int _slider_min;
    int _slider_max;

    std::string _label;
    std::string _str_value;
    std::string _str_min;
    std::string _str_max;

    bool mouse_over_slider_bar(int mouse_x, int mouse_y);
    void check_and_correct_value(double& val);
    void check_and_correct_slider(int& pos);
    void update_slider();
    void set_value_from_slider(int new_pos);

public:
    Slider(int id,
           int x, int y,
           int w, int h,
           double min,
           double max,
           double value,
           std::string label);

    double get_value() {return _value;}

    virtual void handle_mouse_move(const Mouse& mouse);
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void draw() const;
};


class ControlsManager {
    std::vector<UIControl *> controls;
    int generate_unique_id() const;
public:
    ~ControlsManager();

    int add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback);
    int add_button_boolean(int x, int y, int width, int height, std::string label, bool start_state, ActionCallback button_callback_on, ActionCallback button_callback_off);
    int add_textbox(int x, int y, int width, int height, std::string label);
    int add_numericbox(int x, int y, int width, int height, std::string label);
    int add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label);

    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    void handle_keyboard_key_event(char key, KEY_ACTION action);
    void draw() const;

    UIControl* find_by_id(int id);
    void simulate_mouse_action(int id, MOUSE_KEY key, KEY_ACTION action);
};

#endif /* defined(__controls__simplespace__) */

