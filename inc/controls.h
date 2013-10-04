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

#include "mouse_and_keyboard.h"
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

    int get_x() const {return _x;}
    int get_y() const {return _y;}
    int get_width() const {return _w;}
    int get_height() const {return _h;}
    void set_x(const int& x) {_x = x;}
    void set_y(const int& y) {_y = y;}
    void set_width(const int& w) {_w = w;}
    void set_height(const int& h) {_h = h;}

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
    void set_label(const std::string& label) {_label = label;}

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
    virtual void handle_keyboard_key_event(char key, KEY_ACTION action);
    virtual void draw() const;
};


class NumericBox : public UIControl {
    double _value;
    std::string _label;
    bool _label_is_numeric;

    bool _is_active;

    bool _cursor_visible;
    int _cursor_x;
    unsigned long _cursor_char_offset;
    Timer _cursor_timer;

    int _sel_x_begin;
    int _sel_x_end;

    ActionCallback _redraw_notifier;

    bool check_label_is_numeric(const std::string& label);
    double label_to_value(const std::string& label) const;

    void cursor_toggle();
    static void static_wrapper_cursor_toggle(void* param);
    int count_cursor_x(unsigned long char_offset) const;
    unsigned long count_char_offset(int pos_x) const;
    void select_all();
    void cancel_selection();
public:
    NumericBox(int id,
               int x, int y,
               int w, int h,
               double value = 0,
               ActionCallback redraw_notifier = NULL);

    void set_label(const std::string& label); // Does not update _value
    void set_label(const double& value);     // Does not update _value
    void apply_label();

    void set_value(const double& value);     // Updates _label
    double get_value() const {return _value;}

    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void handle_keyboard_key_event(char key, KEY_ACTION action);
    virtual void draw() const;
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

    NumericBox _value_box;

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
           std::string label,
           ActionCallback redraw_notifier);

    double get_value() {return _value;}

    virtual void handle_mouse_move(const Mouse& mouse);
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void handle_keyboard_key_event(char key, KEY_ACTION action);
    virtual void draw() const;
};


class RedrawBox : public UIControl {
    bool active;
    mutable float R;
    mutable float G;
    mutable float B;
public:
    RedrawBox(int id, int x, int y, int w, int h) :
        UIControl(id, x, y, w, h),
        active(false),
        R(0), G(0), B(0) {};

    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    virtual void draw() const;
};


class ControlsManager {
    std::vector<UIControl *> controls;
    int generate_unique_id() const;
    ActionCallback _redraw_notifier;
public:
    ControlsManager(ActionCallback redraw_notifier = NULL);
    ~ControlsManager();

    UIControl* find_by_id(int id);

    int add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback);
    int add_button_boolean(int x, int y, int width, int height, std::string label, bool start_state, ActionCallback button_callback_on, ActionCallback button_callback_off);
    int add_numeric_box(int x, int y, int width, int height, double value);
    int add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label);
    int add_redraw_box(int x, int y, int width, int height);

    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    void handle_keyboard_key_event(char key, KEY_ACTION action);
    void draw() const;

    void shift_conrols_position(const int& x_offset, const int& y_offset);
};

#endif /* defined(__controls__simplespace__) */

