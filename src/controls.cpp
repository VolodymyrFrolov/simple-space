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

// ---- TextBox ----

TextBox::TextBox(int id,
                 int x, int y,
                 int w, int h,
                 std::string label,
                 bool is_numeric) :
    UIControl(id, x, y, w, h),
    _is_active(false),
    _label(label),

    _is_numeric(is_numeric),
    _value(0),
    _label_is_value(false),

    _cursor_visible(false),
    _cursor_timer(650, &TextBox::static_wrapper_cursor_toggle, this, false) {

    if (_is_numeric) {
        if (_label.size() == 0)
            set_label(_value);
        _label_is_value = check_label_is_numeric_and_update_value(_label, _value);
    }
}

bool TextBox::check_label_is_numeric_and_update_value(const std::string& label, double& value) {

    bool ret = false;
    double temp_val;
    char * p_end;

    temp_val = strtod(_label.c_str(), &p_end);

    if ((*p_end) == '\0') {
        value = temp_val;
        ret = true;
    } else {
        ret = false;
    }

    return ret;
}

void TextBox::set_label(const std::string& label) {
    _label = label;
    if (_is_numeric) {
        if (_label.size() == 0)
            set_label(_value);
        _label_is_value = check_label_is_numeric_and_update_value(_label, _value);
    }
}

void TextBox::set_label(const double& value) {
    _value = value;

    std::ostringstream oss;
    oss.precision(3);
    oss << _value;
    _label = oss.str();
    if (_is_numeric)
        _label_is_value = true;
}

void TextBox::handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action) {

    switch (action)
    {
        case KEY_DOWN:
            if (mouse_over_control(mouse.x, mouse.y)) {
                if (!_is_active) {
                    _is_active = true;
                    _cursor_visible = true;
                    _cursor_timer.start();
                }
            } else {
                if (_is_active) {
                    _is_active = false;
                    _cursor_visible = false;
                    _cursor_timer.stop();
                    if (_is_numeric) {
                        set_label(_value);
                    }
                }
            }
        break;

        case KEY_UP:
            // Currently not handling
        break;
    }
}

void TextBox::handle_keyboard_key_event(char key, KEY_ACTION action) {

    switch (action)
    {
        case KEY_DOWN:
            if (_is_active) {
                switch (key)
                {
                    case char(8):   // Delete
                    case char(127): // Backspce
                        _label.clear();
                        break;

                    case char(13):  // Enter
                        _is_active = false;
                        _cursor_visible = false;
                        _cursor_timer.stop();

                        if (_is_numeric) {
                            if (_label.size() == 0)
                                set_label(_value);
                            _label_is_value = check_label_is_numeric_and_update_value(_label, _value);
                        }
                        break;

                    default:
                        if (char(key) > 31 && char(key) < 127) { // Latin keys
                            if (_is_numeric) {
                                switch (key)
                                {
                                    case '0': // Numeric keys
                                    case '1':
                                    case '2':
                                    case '3':
                                    case '4':
                                    case '5':
                                    case '6':
                                    case '7':
                                    case '8':
                                    case '9':
                                    case '+':
                                    case '-':
                                    case '.':
                                    case 'e':
                                    case 'E':
                                    case 'x':
                                    case 'X':
                                        _label.append(1, key);
                                        break;
                                }
                            } else {
                                _label.append(1, key);
                            }
                        }
                        break; // default case
                }
            }
            break; // case KEY_DOWN

        case KEY_UP:
            // Currently not handling
            break;
    }
}

void TextBox::draw() const {

    // Body

    if (_is_numeric && !_label_is_value) {
        if (_is_active)
            glColor3f(1.0f, 0.5f, 0.5f);
        else
            glColor3f(0.9f, 0.4f, 0.4f);
    } else if (_is_active) {
        glColor3f(0.9f, 0.9f, 0.9f);
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

    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_label.c_str())) - 10;
    int font_y = _y + _h/2 + 5;

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    // Cursor

    if (_cursor_visible) {
        glBegin(GL_LINES);
        glVertex2i(_x + _w - 10, _y + _h/2 - 8);
        glVertex2i(_x + _w - 10, _y + _h/2 + 8);
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
               std::string label) :
    UIControl(id, x, y, w, h),
    _is_pressed(false),
    _margin(10),
    _value(value), _value_min(min), _value_max(max),
    _label(label),
    value_textbox(0, _x + _w - 55, _y + 5, 50, 15, "123", true) {

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
    value_textbox.set_label(_value);
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
    return (mouse_x > _x      &&
            mouse_x < _x + _w &&
            mouse_y > _y + 20 &&
            mouse_y < _y + 40);
}

void Slider::handle_mouse_move(const Mouse& mouse) {
    if (_is_pressed && mouse_over_slider_bar(mouse.left_key.down_x, mouse.left_key.down_y)) {
        int slider_new_pos = mouse.x;
        check_and_correct_slider(slider_new_pos);
        set_value_from_slider(slider_new_pos);

        value_textbox.set_label(_value);
    }

    value_textbox.handle_mouse_move(mouse);
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

                value_textbox.set_label(_value);
            }
            break;

            case KEY_UP:
                if (_is_pressed)
                    _is_pressed = false;
            break;
        }
    }

    value_textbox.handle_mouse_key_event(mouse, key, action);

    if (value_textbox.get_value() != this->get_value()) {
        _value = value_textbox.get_value();

        // Prevent value from out of range
        check_and_correct_value(_value);
        value_textbox.set_label(_value);

        update_slider();
    }
}

void Slider::handle_keyboard_key_event(char key, KEY_ACTION action) {

    value_textbox.handle_keyboard_key_event(key, action);

    if (value_textbox.get_value() != this->get_value()) {
        _value = value_textbox.get_value();

        // Prevent value out of range
        check_and_correct_value(_value);
        value_textbox.set_label(_value);

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

    glLineWidth(1);

    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_LINES);
    glVertex2i(_slider_min, _y + _h/2);
    glVertex2i(_slider_max, _y + _h/2);
    glEnd();

    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_min,     _y + _h/2 + 1);
    glVertex2i(_slider_max + 1, _y + _h/2 + 1);
    glVertex2i(_slider_max + 1, _y + _h/2);
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_min, _y + _h/2 + 1);
    glVertex2i(_slider_min, _y + _h/2 - 1);
    glVertex2i(_slider_max, _y + _h/2 - 1);
    glEnd();


    // Slider tip

    if (_is_pressed)
        glColor3f(0.15f, 0.56f, 0.82f);
    else
        glColor3f(0.23f, 0.41f, 0.73f);
    
    glBegin(GL_POLYGON);
    glVertex2i(_slider_pos - 5, _y + _h/2 + 3);
    glVertex2i(_slider_pos - 5, _y + _h/2 - 5);
    glVertex2i(_slider_pos + 5, _y + _h/2 - 5);
    glVertex2i(_slider_pos + 5, _y + _h/2 + 3);
    glVertex2i(_slider_pos,     _y + _h/2 + 9);
    glEnd();

    glLineWidth(1);
    
    glColor3f(0.20f, 0.33f, 0.56f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_pos + 5, _y + _h/2 - 4);
    glVertex2i(_slider_pos + 5, _y + _h/2 + 3);
    glVertex2i(_slider_pos,     _y + _h/2 + 8);
    glVertex2i(_slider_pos - 5, _y + _h/2 + 3);
    glEnd();

    glColor3f(0.37f, 0.55f, 0.87f);
    glBegin(GL_LINE_STRIP);
    glVertex2i(_slider_pos - 4, _y + _h/2 + 4);
    glVertex2i(_slider_pos - 4, _y + _h/2 - 5);
    glVertex2i(_slider_pos + 5, _y + _h/2 - 5);
    glEnd();

    // Value label

    std::string new_label(_label);
    new_label.append(_str_value);

    int font_x = _x + (_w - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)new_label.c_str())) / 2;
    int font_y = _y + _h/3;

    glColor3f(0.0f, 0.0f, 0.0f);
    draw_text_2d(new_label.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    value_textbox.draw();

    // Min & Max label

    font_x = _x + _margin;
    font_y = _y + 5 * _h/6;
    draw_text_2d(_str_min.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);

    font_x = _x + _w - _margin - glutBitmapLength(GLUT_BITMAP_HELVETICA_12, (const unsigned char*)_str_max.c_str());
    draw_text_2d(_str_max.c_str(), font_x, font_y, GLUT_BITMAP_HELVETICA_12);
}

// ---- ControlManager ----

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

int ControlsManager::add_textbox(int x, int y, int width, int height, std::string label, bool is_numeric) {
    int new_id = generate_unique_id();
    controls.push_back(new TextBox(new_id, x, y, width, height, label, is_numeric));
    return new_id;
}

int ControlsManager::add_slider(int x, int y, int width, int height, double min, double max, double value, std::string label) {
    int new_id = generate_unique_id();
    controls.push_back(new Slider(new_id, x, y, width, height, min, max,value, label));
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
