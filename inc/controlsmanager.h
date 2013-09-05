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

typedef void (*ActionCallback)();


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
    virtual void handle_mouse_move(const Mouse& mouse) = 0;
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) = 0;
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
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    virtual void draw() const;
};


class ButtonOnOff : public Button {
    bool _state_on;
    ActionCallback _button_callback_off;

public:
    ButtonOnOff(int id,
                int x, int y,
                int w, int h,
                std::string label,
                bool initial_state,
                ActionCallback action_on,
                ActionCallback action_off) :
    Button(id, x, y, w, h, label, action_on),
    _state_on(initial_state),
    _button_callback_off(action_off) {}
    
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
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

    bool mouse_over_slider_bar(int mouse_x, int mouse_y);
    void correct_value_if_out_of_range(double& val);
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
    virtual void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    virtual void draw() const;
};


class ControlsManager {
    std::vector<UIControl *> controls;
    int generate_unique_id() const;
public:
    ~ControlsManager();
    int add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback);
    int add_button_on_off(int x, int y, int width, int height, std::string label, bool start_state, ActionCallback button_callback_on, ActionCallback button_callback_off);
    int add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label);
    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    void simulate_mouse_action(int id, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action);
    void draw() const;
};

#endif /* defined(__controls_manager__simplespace__) */

