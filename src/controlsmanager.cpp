//
//  controlsmanager.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "controlsmanager.h"
#include <sstream>
#include <algorithm>

void draw_text_2d(const char* str, int x, int y, void* font) {
    glRasterPos2i(x, y);
    for (const char* ch = str; *ch != '\0'; ch++)
        glutBitmapCharacter(font, *ch);
}

// ---- MouseKey ----

void MouseKey::update(bool pressed, int x, int y) {
    is_down = pressed;
    if (pressed) {
        down_x = x;
        down_y = y;
    }
}

// ---- UIControl ----

bool UIControl::mouse_over_control(const int& mouse_x, const int& mouse_y) const {
    return (mouse_x > _x       &&
            mouse_x < _x + _w  &&
            mouse_y > _y       &&
            mouse_y < _y + _h);
}

// ---- Button ----

void Button::handle_mouse_move(const Mouse& mouse) {
    if (mouse_over_control(mouse.x, mouse.y)) {
        if (!_is_mouse_over)
        _is_mouse_over = true;
    } else {
        if (_is_mouse_over)
        _is_mouse_over = false;
    }
}

void Button::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {

    if (mouse_key == MOUSE_LEFT_KEY) {
        switch (mouse_key_action)
        {
            case MOUSE_KEY_DOWN:
                if (!_is_pressed && mouse_over_control(mouse.x, mouse.y)) {
                    _is_pressed = true;
                }
                break;

            case MOUSE_KEY_UP:
                if (_is_pressed &&
                    mouse_over_control(mouse.x, mouse.y) &&
                    mouse_over_control(mouse.left_key.down_x, mouse.left_key.down_y)) {
                    _is_pressed = false;
                    if (_button_callback != NULL)
                        _button_callback();
                } else if (_is_pressed) {
                    _is_pressed = false;
                }
                break;
        }
    }
}

void Button::draw() const {

    // Body

    if (_is_mouse_over)
        glColor3f(0.7f,0.7f,0.7f);
    else
        glColor3f(0.6f,0.6f,0.6f);

    glBegin(GL_QUADS);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();

    // Borders

    glLineWidth(2);

    if (_is_pressed)
        glColor3f(0.4f,0.4f,0.4f);
    else
        glColor3f(0.8f,0.8f,0.8f);

    glBegin(GL_LINE_STRIP);
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x,      _y     );
    glVertex2i(_x,      _y + _h);
    glEnd();

    if (_is_pressed)
        glColor3f(0.8f,0.8f,0.8f);
    else
        glColor3f(0.4f,0.4f,0.4f);

    glBegin(GL_LINE_STRIP);
    glVertex2i(_x,      _y + _h);
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x + _w, _y     );
    glEnd();

    glLineWidth(1);

    // Label

    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str())) / 2;
    int font_y = _y + (_h + 10) / 2;

    if (_is_pressed) {
        font_x += 1;
        font_y += 1;
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- ButtonOnOff ----

void ButtonOnOff::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {
    if (mouse_key == MOUSE_LEFT_KEY) {
        switch (mouse_key_action)
        {
            case MOUSE_KEY_DOWN:
                if (!_is_pressed && mouse_over_control(mouse.x, mouse.y)) {
                    _is_pressed = true;
                }
                break;
                
            case MOUSE_KEY_UP:
                if (_is_pressed &&
                    mouse_over_control(mouse.x, mouse.y) &&
                    mouse_over_control(mouse.left_key.down_x, mouse.left_key.down_y)) {

                    _is_pressed = false;
                    _state_on = !_state_on;
                    if (_state_on) {
                        if (_button_callback != NULL)
                            _button_callback();
                    } else {
                        if (_button_callback_off != NULL)
                            _button_callback_off();
                    }

                } else if (_is_pressed) {
                    _is_pressed = false;
                }
                break;
        }
    }
}

void ButtonOnOff::draw() const {

    // Body
    
    if (_is_mouse_over)
        glColor3f(0.7f,0.7f,0.7f);
    else
        glColor3f(0.6f,0.6f,0.6f);
    
    glBegin(GL_QUADS);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();
    
    // Borders
    
    glLineWidth(2);
    
    if (_is_pressed)
        glColor3f(0.4f,0.4f,0.4f);
    else
        glColor3f(0.8f,0.8f,0.8f);
    
    glBegin(GL_LINE_STRIP);
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x,      _y     );
    glVertex2i(_x,      _y + _h);
    glEnd();
    
    if (_is_pressed)
        glColor3f(0.8f,0.8f,0.8f);
    else
        glColor3f(0.4f,0.4f,0.4f);
    
    glBegin(GL_LINE_STRIP);
    glVertex2i(_x,      _y + _h);
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x + _w, _y     );
    glEnd();
    
    glLineWidth(1);
    
    // Indicator line
    
    int line_x_start = _x + 10;
    int line_x_end = _x + _w - 10;
    int line_y = _y + _h - 5;
    
    if (_is_pressed) {
        line_x_start += 1;
        line_x_end += 1;
        line_y += 1;
    }

    if (_state_on)
        glColor3f(1.0f, 1.0f, 1.0f);
    else
        glColor3f(0.4f, 0.4f, 0.4f);
    
    glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2i(line_x_start, line_y);
    glVertex2i(line_x_end,   line_y);
    glEnd();

    // Label
    
    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str())) / 2;
    int font_y = _y + (_h + 10) / 2;

    if (_is_pressed) {
        font_x += 1;
        font_y += 1;
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- Slider ----

Slider::Slider(int id,
               int x, int y,
               int w, int h,
               double min,
               double max,
               double value,
               std::string label) :
    UIControl(id, x, y, w, h),
    _min(min), _max(max), _value(value),
    _label(label),
    _is_pressed(false) {

    if (_min > _max) {
        _max = _min;
        _value = _min;
        cout << "Error: Slider: min > max" << endl;
    } else if (_value < _min || _value > _max) {
        _value = (_min + _max) / 2;
        cout << "Error: Slider: value out of range" << endl;
    };
}

void Slider::draw() const {

    // Body
    
    glColor3f(0.6f,0.6f,0.6f);
    
    glBegin(GL_QUADS);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();
    
    // Borders
    
    glLineWidth(1);
    
    glColor3f(0.4f,0.4f,0.4f);
    
    glBegin(GL_LINE_LOOP);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();
    
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_LINES);
    glVertex2i(_x,      _y + _h/2     );
    glVertex2i(_x + _w, _y + _h/2     );
    glEnd();
    
    // Slider line
    
    int margin = 10;

    if (_is_pressed)
        glColor3f(0.9f, 0.9f, 0.9f);
    else
        glColor3f(0.8f, 0.8f, 0.8f);

    glBegin(GL_LINES);
    glVertex2i(_x + margin,      _y + _h / 4);
    glVertex2i(_x + _w - margin, _y + _h / 4);
    glEnd();
    
    // Slider pointer

    if (_is_pressed)
        glColor3f(0.9f, 0.9f, 0.9f);
    else
        glColor3f(0.8f, 0.8f, 0.8f);
    
    int pointer_offset;
    if ((_max - _min) == 0) {
        pointer_offset = _w / 2; // Put slider pointer to center
    } else {
        pointer_offset = ((_w - margin * 2) * _value) / (_max - _min);
    }
    
    glLineWidth(10);
    glBegin(GL_LINES);
    glVertex2i(_x + pointer_offset + margin, _y + _h/4 - 5);
    glVertex2i(_x + pointer_offset + margin, _y + _h/4 + 5);
    glEnd();
    
    // Label
    
    std::string new_label(_label);
    new_label.append((std::ostringstream() << _value).str());
    
    // Same (get string from value) done step by step
    //std::string value_string;
    //std::ostringstream oss;
    //oss << _value;
    //value_string = oss.str();
    
    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)new_label.c_str())) / 2;
    int font_y = _y + ((_h * 3) / 4) + 5;
    
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(new_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

}

void Slider::handle_mouse_move(const Mouse& mouse) {
    
}

void Slider::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {
    
}

// ---- ControlManager ----

ControlsManager::~ControlsManager() {
    if (controls.size() != 0) {
        std::for_each(controls.begin(), controls.end(), [](UIControl* b) {delete b; b = NULL;});
    }
}

int ControlsManager::generate_unique_id() const {
    int new_id = 0;
    while (std::any_of(controls.begin(), controls.end(), [&](UIControl* b) {return b->get_id() == new_id;})) {
        ++new_id;
    }
    return new_id;
}

int ControlsManager::add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback) {
    int new_id = generate_unique_id();
    controls.push_back(new Button(new_id, x, y, width, height, label, button_callback));
    return new_id;
}

int ControlsManager::add_button_on_off(int x, int y, int width, int height, std::string label, bool start_state, ActionCallback button_callback_on, ActionCallback button_callback_off) {
    int new_id = generate_unique_id();
    controls.push_back(new ButtonOnOff(new_id, x, y, width, height, label, start_state, button_callback_on, button_callback_off));
    return new_id;
}

int ControlsManager::add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label) {
    int new_id = generate_unique_id();
    controls.push_back(new Slider(new_id, x, y, width, height, min, max,value, label));
    return new_id;
}

void ControlsManager::handle_mouse_move(const Mouse& mouse) {
    for (std::vector<UIControl *>::iterator it = controls.begin(), it_end = controls.end(); it != it_end; ++it)
        (*it)->handle_mouse_move(mouse);
}

void ControlsManager::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {
    for (std::vector<UIControl *>::iterator it = controls.begin(), it_end = controls.end(); it != it_end; ++it)
        (*it)->handle_mouse_key_event(mouse, mouse_key, mouse_key_action);
}

void ControlsManager::draw() const {
    for (std::vector<UIControl *>::const_iterator it = controls.begin(), it_end = controls.end(); it != it_end; ++it)
        (*it)->draw();
}