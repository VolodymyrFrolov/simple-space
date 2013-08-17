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

ControlsManager::ControlsManager() : buttons_number_max(50) {}

void ControlsManager::draw_label(std::string label, int x, int y, void* font) const {
    glRasterPos2i(x, y);
    for (const char * ch = label.c_str(); *ch != '\0'; ch++)
        glutBitmapCharacter(font, *ch);
}

bool ControlsManager::mouse_over_button(const Button& button, const int& mouse_x, const int& mouse_y) const {
    return (mouse_x > button.x                 &&
            mouse_x < button.x + button.width  &&
            mouse_y > button.y                 &&
            mouse_y < button.y + button.height);
}

void ControlsManager::add_button(int x, int y, int width, int height, std::string label, ButtonCallback button_callback) {
    if (buttons.size() < buttons_number_max) {
        unsigned int new_id = 0;
        while (std::any_of(buttons.begin(), buttons.end(), [&](Button b) {return b.id == new_id;}))
            ++new_id;
        buttons.push_back({new_id, x, y, width, height, false, false, label, button_callback});
    } else {
        std::cout << "Can't add control: max controls number reached!" << endl;
    }
}

void ControlsManager::handle_mouse_move(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (mouse_over_button(*it, mouse.x, mouse.y))
            it->is_mouse_over = true;
        else
            it->is_mouse_over = false;
    }
}

void ControlsManager::handle_button_down(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (mouse_over_button(*it, mouse.x, mouse.y)) {
            it->is_pressed = true;
        }
    }
}

void ControlsManager::handle_button_up(const Mouse& mouse) {
    for (std::vector<Button>::iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (mouse_over_button(*it, mouse.x, mouse.y) &&
            mouse_over_button(*it, mouse.left_key_down_x, mouse.left_key_down_y)) {
            it->is_pressed = false;
            if (it->button_callback != NULL)
                it->button_callback();
        } else if (it->is_pressed) {
            it->is_pressed = false;
        }
    }
}

void ControlsManager::draw_buttons() const {
    for (std::vector<Button>::const_iterator it = buttons.begin(), it_end = buttons.end(); it != it_end; ++it) {
        if (it->is_mouse_over)
            glColor3f(0.7f,0.7f,0.7f);
        else
            glColor3f(0.6f,0.6f,0.6f);

        glBegin(GL_QUADS);
            glVertex2i(it->x,             it->y              );
            glVertex2i(it->x + it->width, it->y              );
            glVertex2i(it->x + it->width, it->y + it->height );
            glVertex2i(it->x,             it->y + it->height );
        glEnd();

        glLineWidth(2);

        if (it->is_pressed) 
            glColor3f(0.4f,0.4f,0.4f);
        else 
            glColor3f(0.8f,0.8f,0.8f);

        glBegin(GL_LINE_STRIP);
            glVertex2i(it->x + it->width, it->y              );
            glVertex2i(it->x,             it->y              );
            glVertex2i(it->x,             it->y + it->height );
        glEnd();

        if (it->is_pressed)
            glColor3f(0.8f,0.8f,0.8f);
        else 
            glColor3f(0.4f,0.4f,0.4f);

        glBegin(GL_LINE_STRIP);
            glVertex2i(it->x,             it->y + it->height);
            glVertex2i(it->x + it->width, it->y + it->height);
            glVertex2i(it->x + it->width, it->y             );
        glEnd();

        glLineWidth(1);

        int font_x = it->x + (it->width - glutBitmapLength(GLUT_BITMAP_HELVETICA_10, (const unsigned char*)it->label.c_str())) / 2;
        int font_y = it->y + (it->height + 10) / 2;
        
        if (it->is_pressed) {
            font_x += 1;
            font_y += 1;
        }

        glColor3f(0.0f, 0.0f, 0.0f);
        draw_label(it->label, font_x, font_y, GLUT_BITMAP_HELVETICA_12);
    }
}
