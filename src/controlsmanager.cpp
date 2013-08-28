//
//  controlsmanager.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "controlsmanager.h"
#include <set>
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

bool UIControl::mouse_over_button(const int& mouse_x, const int& mouse_y) const {
    return (mouse_x > _x       &&
            mouse_x < _x + _w  &&
            mouse_y > _y       &&
            mouse_y < _y + _h);
}

// ---- Button ----

void Button::handle_mouse_move(const Mouse& mouse) {
    if (mouse_over_button(mouse.x, mouse.y)) {
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
                if (!_is_pressed && mouse_over_button(mouse.x, mouse.y)) {
                    _is_pressed = true;
                }
                break;

            case MOUSE_KEY_UP:
                if (_is_pressed &&
                    mouse_over_button(mouse.x, mouse.y) &&
                    mouse_over_button(mouse.left_key.down_x, mouse.left_key.down_y)) {
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

    glColor3f(0.1f, 0.1f, 0.1f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- ButtonOnOff ----

void ButtonOnOff::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {
    if (mouse_key == MOUSE_LEFT_KEY) {
        switch (mouse_key_action)
        {
            case MOUSE_KEY_DOWN:
                if (!_is_pressed && mouse_over_button(mouse.x, mouse.y)) {
                    _is_pressed = true;
                }
                break;
                
            case MOUSE_KEY_UP:
                if (_is_pressed &&
                    mouse_over_button(mouse.x, mouse.y) &&
                    mouse_over_button(mouse.left_key.down_x, mouse.left_key.down_y)) {

                    _is_pressed = false;
                    _is_on = !_is_on;
                    if (_is_on) {
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

    if (_is_on)
        glColor3f(1.0f, 1.0f, 1.0f);
    else
        glColor3f(0.3f, 0.3f, 0.3f);
    
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

    glColor3f(0.2f, 0.3f, 0.2f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- ControlManager ----

int ControlsManager::add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback) {
    int new_id = 0;
    while (std::any_of(buttons.begin(), buttons.end(), [&](Button b) {return b.get_id() == new_id;}))
        ++new_id;
    buttons.push_back(Button(new_id, x, y, width, height, label, button_callback));
    return new_id;
}

void ControlsManager::handle_mouse_move(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->handle_mouse_move(mouse);
}

void ControlsManager::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY mouse_key, MOUSE_KEY_ACTION mouse_key_action) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->handle_mouse_key_event(mouse, mouse_key, mouse_key_action);
}

void ControlsManager::draw_buttons() const {
    for (std::vector<Button>::const_iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->draw();
}

int ControlsManager::find_id_by_label(std::string label) const {
    int ret = -1;
    for (std::vector<Button>::const_iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (it->get_label() == label) {
            ret = it->get_id();
            break;
        }
    }
    return ret;
}

void ControlsManager::set_label_by_id(std::string label, int id) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (it->get_id() == id) {
            it->set_label(label);
            break;
        }
    }
}
