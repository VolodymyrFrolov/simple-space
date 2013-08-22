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

void Button::handle_button_down(const Mouse& mouse) {
    // Refuse to become pressed if one of mouse buttons is already pressed
    if (!_is_pressed &&
        mouse_over_button(mouse.x, mouse.y) &&
        !(mouse.left_key_down && mouse.right_key_down))
        _is_pressed = true;
}

void Button::handle_button_up(const Mouse& mouse) {
    // Refuse to become released if one of buttons is still pressed
    if (!(mouse.left_key_down || mouse.right_key_down)) {
        if (_is_pressed &&
            mouse_over_button(mouse.x, mouse.y) &&
            (mouse_over_button(mouse.left_key_down_x, mouse.left_key_down_y) ||
                mouse_over_button(mouse.right_key_down_x, mouse.right_key_down_y))) {
            _is_pressed = false;
            if (_button_callback != NULL)
                _button_callback();
        } else if (_is_pressed) {
            _is_pressed = false;
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

void ControlsManager::add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback) {
    unsigned int new_id = 0;
    while (std::any_of(buttons.begin(), buttons.end(), [&](Button b) {return b.get_id() == new_id;}))
        ++new_id;
    buttons.push_back(Button(new_id, x, y, width, height, label, button_callback));
}

void ControlsManager::handle_mouse_move(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->handle_mouse_move(mouse);
}

void ControlsManager::handle_button_down(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->handle_button_down(mouse);
}

void ControlsManager::handle_button_up(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->handle_button_up(mouse);
}

void ControlsManager::draw_buttons() const {
    for (std::vector<Button>::const_iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it)
        it->draw();
}
