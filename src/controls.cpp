//
//  controls.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 16.08.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "controls.h"
#include <sstream>
#include <algorithm>
#include <thread>

void draw_text_2d(const char* str, int x, int y, void* font) {
    glRasterPos2i(x, y);
    for (const char* ch = str; *ch != '\0'; ch++)
        glutBitmapCharacter(font, *ch);
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

void Button::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {

    if (key == MOUSE_LEFT_KEY) {
        switch (action)
        {
            case KEY_DOWN:
                if (mouse_over_control(mouse.x, mouse.y) && !_is_pressed) {
                    _is_pressed = true;
                }
                break;

            case KEY_UP:
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

// ---- ButtonBoolean ----

void ButtonBoolean::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {

    if (key == MOUSE_LEFT_KEY) {
        switch (action)
        {
            case KEY_DOWN:
                if (mouse_over_control(mouse.x, mouse.y) && !_is_pressed) {
                    _is_pressed = true;
                }
                break;

            case KEY_UP:
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

// Handle space button as trigger
void ButtonBoolean::handle_keyboard_key_event(char key, KEY_ACTION action) {

    if ((key == ' ') && (action == KEY_DOWN)) {
        _state_on = !_state_on;
        if (_state_on) {
            if (_button_callback != NULL)
                _button_callback();
        } else {
            if (_button_callback_off != NULL)
                _button_callback_off();
        }
    }
}

void ButtonBoolean::draw() const {

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
        glColor3f(0.3f, 0.9f, 0.5f);
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

// ---- NumericBox ----

NumericBox::NumericBox(int id,
                       int x, int y,
                       int w, int h,
                       double value,
                       ActionCallback redraw_notifier) :
    UIControl(id, x, y, w, h),
    _value(value),
    _label_is_numeric(true),
    _is_active(false),

    _cursor_visible(false),
    _cursor_x(0),
    _cursor_char_offset(0),
    _cursor_timer(650, &NumericBox::static_wrapper_cursor_toggle, this, false),
    _sel_x_begin(0),
    _sel_x_end(0),
    _redraw_notifier(redraw_notifier) {

    set_value(_value);
}

void NumericBox::cursor_toggle() {
    _cursor_visible = !_cursor_visible;
    if (_redraw_notifier != NULL)
        _redraw_notifier();
}

void NumericBox::static_wrapper_cursor_toggle(void* param) {
    ((NumericBox*)param)->cursor_toggle();
}

bool NumericBox::check_label_is_numeric(const std::string& label) {

    char* p_end;
    strtod(_label.c_str(), &p_end);

    if (((*p_end) == '\0') && (label.size() != 0)) {
        if (!_label_is_numeric)
            _label_is_numeric = true;
    } else {
        if (_label_is_numeric)
            _label_is_numeric = false;
    }

    return _label_is_numeric;
}

double NumericBox::label_to_value(const std::string& label) const {

    double ret;
    char* p_end;

    ret = strtod(_label.c_str(), &p_end);

    if (((*p_end) != '\0') && (label.size() != 0))
        cout << "Warning: [NumericBox] at getting value - parsing failed at:" << *p_end << " or label was empty" << endl;

    return ret;
}

void NumericBox::set_label(const std::string& label) {

    _label = label;
    check_label_is_numeric(_label);
}

void NumericBox::set_label(const double& value) {

    std::ostringstream oss;
    oss.precision(3);
    oss << value;
    _label = oss.str();

    if (!_label_is_numeric)
        _label_is_numeric = true;
}

void NumericBox::apply_label() {

    if (_label_is_numeric)
        _value = label_to_value(_label);
}

void NumericBox::set_value(const double& value) {
    _value = value;
    set_label(_value);
}

int NumericBox::count_cursor_x(unsigned long char_offset) const {
    int label_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str());
    int label_begin = _x + (_w - label_width)/2;

    const char* string_before_cursor = std::string(_label.begin(), _label.begin() + char_offset).c_str();
    int cursor_offset = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)string_before_cursor);

    return label_begin + cursor_offset + 1; // +1pixel margin for cursor
}

unsigned long NumericBox::count_char_offset(int pos_x) const {
    unsigned long ret;
    int label_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str());
    int label_begin = _x + (_w - label_width)/2;

    if (pos_x < label_begin || _label.size() == 0) {
        ret = 0;
    } else if (pos_x > (label_begin + label_width)) {
        ret = _label.size();
    } else {
        int cursor_offset = pos_x - label_begin;

        std::string temp_str = _label;
        int temp_str_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)temp_str.c_str());

        while (temp_str_width > cursor_offset) {
            temp_str.erase(temp_str.end()-1);
            temp_str_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)temp_str.c_str());
        }

        ret = temp_str.size();
    }

    return ret;
}

void NumericBox::select_all() {
    int label_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str());
    int label_begin = _x + (_w - label_width)/2;
    _sel_x_begin = label_begin;
    _sel_x_end = label_begin + label_width;
}

void NumericBox::cancel_selection() {
    _sel_x_begin = 0;
    _sel_x_end = 0;
}

void NumericBox::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {

    switch (action)
    {
        case KEY_DOWN:
            if (mouse_over_control(mouse.x, mouse.y)) {

                if (!_is_active) {
                    select_all();
                    _is_active = true;
                } else {
                    _cursor_char_offset = count_char_offset(mouse.x);
                    _cursor_x = count_cursor_x(_cursor_char_offset);

                    cancel_selection();
                    _cursor_visible = true;
                    _cursor_timer.start();
                }
            } else {
                if (_is_active) {
                    _is_active = false;
                    _cursor_visible = false;
                    _cursor_timer.stop();

                    if (_label_is_numeric)
                        apply_label();
                    else
                        set_label(_value);
                    }
            }
        break;

        case KEY_UP:
            // Currently not handling
        break;
    }
}

void NumericBox::handle_keyboard_key_event(char key, KEY_ACTION action) {

    switch (action)
    {
        case KEY_DOWN:
            if (_is_active) {

                switch (key)
                {
                    case char(8):   // Backspce
                        if(_label.size() > 0 && _cursor_char_offset > 0) {
                            _label.erase(_label.begin() + _cursor_char_offset - 1);

                            --_cursor_char_offset;
                            _cursor_x = count_cursor_x(_cursor_char_offset);

                            if (_label.size() > 0) {
                                check_label_is_numeric(_label);
                            } else {
                                // Force "true" for empty label
                                _label_is_numeric = true;
                            }
                        }
                        break;

                    case char(127): // Delete
                        if(_label.size() > 0 && (_cursor_char_offset < _label.size())) {
                            _label.erase(_label.begin() + _cursor_char_offset);

                            _cursor_x = count_cursor_x(_cursor_char_offset);

                            if (_label.size() > 0) {
                                check_label_is_numeric(_label);
                            } else {
                                // Force "true" for empty label
                                _label_is_numeric = true;
                            }
                        }
                        break;

                    case char(13):  // Enter
                        _is_active = false;
                        _cursor_visible = false;
                        _cursor_timer.stop();

                        if (!_label_is_numeric || _label.size() == 0)
                            set_label(_value); // Reset label
                        _value = label_to_value(_label);
                        break;

                    case ARROW_LEFT:
                    case ARROW_UP:
                        _cursor_timer.stop();
                        if (_cursor_char_offset > 0) {
                            --_cursor_char_offset;
                            _cursor_x = count_cursor_x(_cursor_char_offset);
                        }
                        _cursor_visible = true;
                        _cursor_timer.start();
                        break;

                    case ARROW_RIGHT:
                    case ARROW_DOWN:
                        _cursor_timer.stop();
                        if (_cursor_char_offset < _label.size()) {
                            ++_cursor_char_offset;
                            _cursor_x = count_cursor_x(_cursor_char_offset);
                        }
                        _cursor_visible = true;
                        _cursor_timer.start();
                        break;

                    case HOME_KEY:
                        _cursor_timer.stop();
                        if (_cursor_char_offset != 0) {
                            _cursor_char_offset = 0;
                            _cursor_x = count_cursor_x(_cursor_char_offset);
                        }
                        _cursor_visible = true;
                        _cursor_timer.start();
                        break;

                    case END_KEY:
                        _cursor_timer.stop();
                        if (_cursor_char_offset != _label.size()) {
                            _cursor_char_offset = _label.size();
                            _cursor_x = count_cursor_x(_cursor_char_offset);
                        }
                        _cursor_visible = true;
                        _cursor_timer.start();
                        break;

                    default: // Latin keys (32-126), except space (32)
                        if (char(key) > 32 && char(key) < 127 && _label.size() < 8) {
                            _label.insert(_label.begin() + _cursor_char_offset, key);

                            ++_cursor_char_offset;
                            _cursor_x = count_cursor_x(_cursor_char_offset);

                            check_label_is_numeric(_label);
                        }
                        break;
                }
            }
            break; // case KEY_DOWN

        case KEY_UP:
            // Currently not handling
            break;
    }
}

void NumericBox::draw() const {

    // Body

    if (_is_active) {
        if (_label_is_numeric)
            glColor3f(0.9f, 0.9f, 0.9f);
        else
            glColor3f(1.0f, 0.6f, 0.6f);
    } else {
        glColor3f(0.8f, 0.8f, 0.8f);
    }

    glBegin(GL_QUADS);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();

    // Borders

    glLineWidth(1);

    if (_is_active)
        glColor3f(0.8f, 0.8f, 0.8f);
    else
        glColor3f(0.7f, 0.7f, 0.7f);

    glBegin(GL_LINE_STRIP);
    glVertex2i(_x,      _y + _h);
    glVertex2i(_x     , _y     );
    glVertex2i(_x + _w, _y     );
    glEnd();

    if (_is_active)
        glColor3f(1.0f, 1.0f, 1.0f);
    else
        glColor3f(0.9f, 0.9f, 0.9f);

    glBegin(GL_LINE_STRIP);
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();

    // Label

    int label_width = glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str());
    int font_x = _x + (_w - label_width)/2;
    int font_y = _y + _h/2 + 5;

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    // Cursor

    if (_cursor_visible) {
        glBegin(GL_LINES);
        glVertex2i(_cursor_x, _y + _h/2 - 8);
        glVertex2i(_cursor_x, _y + _h/2 + 8);
        glEnd();
    }

    // Selection

    if (_is_active && (_sel_x_end - _sel_x_begin) != 0) {

        int begin, end;
        if ((_sel_x_end - _sel_x_begin) > 0) {
            begin = _sel_x_begin;
            end = _sel_x_end;
        } else {
            begin = _sel_x_end;
            end = _sel_x_begin;
        }

        glColor4f(0.1f, 0.1f, 0.3f,  0.3f);
        glBegin(GL_QUADS);
        glVertex2i(begin, font_y - 10);
        glVertex2i(end,   font_y - 10);
        glVertex2i(end,   font_y + 1 );
        glVertex2i(begin, font_y + 1 );
        glEnd();
    }
}

// ---- Slider ----

Slider::Slider(int id,
               int x, int y,
               int w, int h,
               double min,
               double max,
               double value,
               std::string label,
               ActionCallback redraw_notifier) :
    UIControl(id, x, y, w, h),
    _is_pressed(false),
    _margin(10),
    _value(value), _value_min(min), _value_max(max),
    _label(label),
    _value_box(0, _x + (_w - 50)/2, _y + 4*_h/6, 50, 15, 0, redraw_notifier) {

    // Check range (done once)
    if (_value_min > _value_max) {
         cout << "Error: slider value_min > value_max" << endl;
        _value_max = _value_min;
    }

    // Check value
    check_and_correct_value(_value);

    // Init borders position for drawing
    _slider_min = _x + _margin;
    _slider_max = _x + _w - _margin;

    // Init labels for borders
    std::ostringstream oss;
#   ifdef __APPLE__
        _str_min = (std::ostringstream() << _value_min).str();
#   else
        _str_min = static_cast<std::ostringstream&>(std::ostringstream() << _value_min).str();
#   endif

    oss.clear();
    oss.str(std::string());
#   ifdef __APPLE__
        _str_max = (std::ostringstream() << _value_max).str();
#   else
        _str_max = static_cast<std::ostringstream&>(std::ostringstream() << _value_max).str();
#   endif

    // updating slider, using value and borders
    update_slider();
    _value_box.set_value(_value);
}

void Slider::check_and_correct_value(double& val) {

    if (val < _value_min) {
        cout << "Warning: slider value = " << val << " < min , but has been corrected" << endl;
        val = _value_min;
    }
    if (val > _value_max) {
        cout << "Warning: slider value = " << val << " > max , but has been corrected" << endl;
        val = _value_max;
    }
}

void Slider::check_and_correct_slider(int& pos) {
    if (pos < _slider_min)
        pos = _slider_min;
    else if (pos > _slider_max)
        pos = _slider_max;
}

void Slider::update_slider() {

    int screen_diff = _slider_max - _slider_min;
    double value_diff = _value_max - _value_min;
    double value_relative = _value - _value_min;

    _slider_pos = _x + _margin + (screen_diff * value_relative) / value_diff;

    //There are two ways to get string from value:
    // 1 - in one line
    //_str_value = static_cast<std::ostringstream&>(std::ostringstream() << _value).str();

    // 2 - step by step
    std::ostringstream oss;
    //oss << std::fixed
    //oss << std::scientific;
    //oss.unsetf(std::ios::scientific);
    oss.precision(3);

    oss << _value;
    _str_value = oss.str();
}

void Slider::set_value_from_slider(int new_pos) {

    int screen_diff = _slider_max - _slider_min;
    double value_diff = _value_max - _value_min;
    int pos_relative = new_pos - _slider_min;

    _value = _value_min + (value_diff * pos_relative) / screen_diff;

    update_slider();
}

bool Slider::mouse_over_slider_bar(int mouse_x, int mouse_y) {
    return (mouse_x > _x                   &&
             mouse_x < _x + _w              &&
             mouse_y > _y + 3*_h/6 - 10     &&
             mouse_y < _y + 3*_h/6 + 10);
}

void Slider::handle_mouse_move(const Mouse& mouse) {
    if (_is_pressed && mouse_over_slider_bar(mouse.left_key.down_x, mouse.left_key.down_y)) {
        int slider_new_pos = mouse.x;
        check_and_correct_slider(slider_new_pos);
        set_value_from_slider(slider_new_pos);

        _value_box.set_value(_value);
    }

    _value_box.handle_mouse_move(mouse);
}

void Slider::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {

    if (key == MOUSE_LEFT_KEY) {
        switch (action)
        {
            case KEY_DOWN:
            if (mouse_over_slider_bar(mouse.x, mouse.y)) {
                int slider_new_pos = mouse.x;
                check_and_correct_slider(slider_new_pos);
                if (!_is_pressed)
                    _is_pressed = true;
                set_value_from_slider(slider_new_pos);

                _value_box.set_value(_value);
            }
            break;

            case KEY_UP:
                if (_is_pressed)
                    _is_pressed = false;
            break;
        }
    }

    _value_box.handle_mouse_key_event(mouse, key, action);

    if (_value_box.get_value() != this->get_value()) {

        // Prevent value from out of range
        double temp = _value_box.get_value();
        check_and_correct_value(temp);
        _value_box.set_value(_value);

        update_slider();
    }
}

void Slider::handle_keyboard_key_event(char key, KEY_ACTION action) {

    _value_box.handle_keyboard_key_event(key, action);

    if (_value_box.get_value() != this->get_value()) {

        // Prevent value from out of range
        double temp = _value_box.get_value();
        check_and_correct_value(temp);
        _value = temp;
        if (_value_box.get_value() != temp)
            _value_box.set_value(temp);

        update_slider();
    }
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
    glBegin(GL_LINE_STRIP);
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();

    glColor3f(0.8f,0.8f,0.8f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_x,      _y + _h);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glEnd();

    // Slider line

    int slider_y = _y + 3*_h/6;

    glLineWidth(1);

    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2i(_slider_min, slider_y);
    glVertex2i(_slider_max, slider_y);
    glEnd();

    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_min,     slider_y + 1);
    glVertex2i(_slider_max + 1, slider_y + 1);
    glVertex2i(_slider_max + 1, slider_y);
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_min, slider_y + 1);
    glVertex2i(_slider_min, slider_y - 1);
    glVertex2i(_slider_max, slider_y - 1);
    glEnd();


    // Slider tip

    if (_is_pressed)
        glColor3f(0.15f, 0.56f, 0.82f);
    else
        glColor3f(0.23f, 0.41f, 0.73f);
    
    glBegin(GL_POLYGON);
    glVertex2i(_slider_pos - 5, slider_y + 3);
    glVertex2i(_slider_pos - 5, slider_y - 5);
    glVertex2i(_slider_pos + 5, slider_y - 5);
    glVertex2i(_slider_pos + 5, slider_y + 3);
    glVertex2i(_slider_pos,     slider_y + 9);
    glEnd();

    glLineWidth(1);
    
    glColor3f(0.20f, 0.33f, 0.56f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_pos + 5, slider_y - 4);
    glVertex2i(_slider_pos + 5, slider_y + 3);
    glVertex2i(_slider_pos,     slider_y + 8);
    glVertex2i(_slider_pos - 5, slider_y + 3);
    glEnd();

    glColor3f(0.37f, 0.55f, 0.87f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_pos - 4, slider_y + 4);
    glVertex2i(_slider_pos - 4, slider_y - 5);
    glVertex2i(_slider_pos + 5, slider_y - 5);
    glEnd();

    // Value label

    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str())) / 2;
    int font_y = _y + _h/4;

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    _value_box.draw();

    // Min & Max label

    font_x = _x + _margin;
    font_y = _y + 7*_h/8;
    draw_text_2d(_str_min.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    font_x = _x + _w - _margin - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_str_max.c_str());
    draw_text_2d(_str_max.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- TestBox ----

void RedrawBox::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {
    if (mouse_over_control(mouse.x, mouse.y) && action == KEY_DOWN)
        active = !active;
}

void RedrawBox::draw() const {

    if (active) {
        R += 0.1;
        G += 0.2;
        B += 0.3;
        if (R > 1) R = 0;
        if (G > 1) G = 0;
        if (B > 1) B = 0;
    }

    glColor3f(R, G, B);
    glBegin(GL_QUADS);
    glVertex2i(_x,      _y     );
    glVertex2i(_x + _w, _y     );
    glVertex2i(_x + _w, _y + _h);
    glVertex2i(_x,      _y + _h);
    glEnd();
}

// ---- ControlManager ----

ControlsManager::ControlsManager(ActionCallback redraw_notifier) :
    _redraw_notifier(redraw_notifier)
{}

ControlsManager::~ControlsManager() {
    if (controls.size() != 0) {
        std::for_each(controls.begin(), controls.end(), [](UIControl* b) {delete b; b = NULL;});
    }
}

int ControlsManager::generate_unique_id() const {
    int new_id = 0;
    while (std::any_of(controls.begin(), controls.end(), [&](UIControl* b) {return b->get_id() == new_id;} )) {
        ++new_id;
    }
    return new_id;
}

UIControl* ControlsManager::find_by_id(int id) {
    std::vector<UIControl *>::iterator it = std::find_if(controls.begin(), controls.end(), [&](UIControl* b) {return b->get_id() == id;});
    if (it != controls.end())
        return *it;
    else
        return NULL;
}

int ControlsManager::add_button(int x, int y, int width, int height, std::string label, ActionCallback button_callback) {
    int new_id = generate_unique_id();
    controls.push_back(new Button(new_id, x, y, width, height, label, button_callback));
    return new_id;
}

int ControlsManager::add_button_boolean(int x, int y, int width, int height, std::string label, bool start_state, ActionCallback button_callback_on, ActionCallback button_callback_off) {
    int new_id = generate_unique_id();
    controls.push_back(new ButtonBoolean(new_id, x, y, width, height, label, start_state, button_callback_on, button_callback_off));
    return new_id;
}

int ControlsManager::add_numeric_box(int x, int y, int width, int height, double value) {
    int new_id = generate_unique_id();
    controls.push_back(new NumericBox(new_id, x, y, width, height, value, _redraw_notifier));
    return new_id;
}

int ControlsManager::add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label) {
    int new_id = generate_unique_id();
    controls.push_back(new Slider(new_id, x, y, width, height, min, max,value, label, _redraw_notifier));
    return new_id;
}

int ControlsManager::add_redraw_box(int x, int y, int width, int height) {
    int new_id = generate_unique_id();
    controls.push_back(new RedrawBox(new_id, x, y, width, height));
    return new_id;
}

void ControlsManager::handle_mouse_move(const Mouse& mouse) {
    std::for_each(controls.begin(), controls.end(), [&](UIControl* c) {c->handle_mouse_move(mouse);} );
}

void ControlsManager::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {
    std::for_each(controls.begin(), controls.end(), [&](UIControl* c) {c->handle_mouse_key_event(mouse, key, action);} );
}

void ControlsManager::handle_keyboard_key_event(char key, KEY_ACTION action) {
    std::for_each(controls.begin(), controls.end(), [&](UIControl* c) {c->handle_keyboard_key_event(key, action);} );
}

void ControlsManager::draw() const {
    std::for_each(controls.begin(), controls.end(), [&](UIControl* c) {c->draw();} );
}

void ControlsManager::shift_conrols_position(const int& x_offset, const int& y_offset) {
    std::for_each(controls.begin(), controls.end(), [&x_offset, &y_offset](UIControl* c) {
        if (x_offset != 0)
            c->set_x(c->get_x() + x_offset);
        if (y_offset != 0)
            c->set_y(c->get_y() + y_offset);
    } );
}
