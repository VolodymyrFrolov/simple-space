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

// ---- MouseKey ----

void MouseKey::update(bool pressed, int x, int y) {
    is_down = pressed;
    if (pressed) {
        down_x = x;
        down_y = y;
    }
}

// ---- Button ----

bool Button::mouse_over_button(const int& mouse_x, const int& mouse_y) const {
    return (mouse_x > _x       &&
            mouse_x < _x + _w  &&
            mouse_y > _y       &&
            mouse_y < _y + _h);
}

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
                if (_is_pressed && mouse_over_button(mouse.left_key.down_x, mouse.left_key.down_y)) {
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

    // Button body

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

    // Button borders

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

    // Button label

    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)_label.c_str())) / 2;
    int font_y = _y + (_h + 10) / 2;

    if (_is_pressed) {
        font_x += 1;
        font_y += 1;
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(font_x, font_y);
    for (const char * ch = _label.c_str(); *ch != '\0'; ch++)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *ch);
}

// ---- ControlManager ----

int ControlsManager::add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback) {
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
