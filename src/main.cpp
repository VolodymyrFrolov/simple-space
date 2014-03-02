//
//  main.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 07.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include <iostream>
using std::cout;
using std::endl;
#include <vector>
#include <string>
#include <sstream>
#include <memory> // std::unique_ptr
#include <sys/time.h> // gettimeofday()

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/glu.h>
    //#include <GLUT/glut.h>
    #include <GL/freeglut.h>
#elif __linux__
    //#include <GL/glut.h>
    #include <GL/freeglut.h>
#else
    // Unsupproted platform
#endif

//#include <ft2build.h>
//#include FT_FREETYPE_H

#include <stdlib.h> // rand()
#include <time.h>

#include "controls.h"
#include "simplespace.h"
#include "planet.h"
#include "physics.h"
#include "timer.h"

//FT_Library  ft_library; // FreeType library handler
//FT_Face     face;       // Face object handler

const int frame_rate = 60;

int window_width = 1200;
int window_height = 600;

const int scene_min_width = 100;
const int menu1_width = 200; // Left menu
const int menu2_width = 100; // Right menu

const int window_min_width = menu1_width + menu2_width + scene_min_width;
const int window_min_height = 600;

int main_window_id = -1;
int radius_slider_id = -1;
int mass_slider_id = -1;

int model_speed = 1;
int model_scale = 200000;

bool simulation_on = true;
bool need_to_render_menu1 = true;
bool need_to_render_menu2 = true;
bool need_to_render_scene = true;

// These functions are intented to be passed as callbacks
void notify_to_update_menu1() {need_to_render_menu1 = true;}
void notify_to_update_menu2() {need_to_render_menu2 = true;}
void notify_to_update_scene() {need_to_render_scene = true;}

void notify_to_update_all() {
    notify_to_update_menu1();
    notify_to_update_menu2();
    notify_to_update_scene();
}

Mouse mouse;

bool mass_modifier_key_down = false;
bool rad_modifier_key_down = false;

const double default_planet_mass = 1e29;
const double default_planet_rad = 2e6;

Planet next_planet;

std::stringstream ss;
std::string str;

enum AliasMode {
    ALIAS_MODE_ALIASED,
    ALIAS_MODE_ANTIALIASED,
    ALIAS_MODE_MULTISAMPLE
};

AliasMode gMode = ALIAS_MODE_MULTISAMPLE;

// Creating global smart pointers (unique_ptr in std)
std::unique_ptr<SimpleSpace> pSimpleSpace(new SimpleSpace(1000/frame_rate));
std::unique_ptr<ControlsManager> pControlsLeft(new ControlsManager(notify_to_update_menu1));
std::unique_ptr<ControlsManager> pControlsRight(new ControlsManager(notify_to_update_menu2));
std::unique_ptr<FPSCounter> pFPSCounter(new FPSCounter);
std::unique_ptr<StopWatch> pStopWatch(new StopWatch(false));

// Temp stopwatch to count time of rendring frame
//std::unique_ptr<StopWatch> pStopWatch_render(new StopWatch(false));

void initRendering();
void onTimer(int next_timer_tick);

void render_window();
void handle_window_visibility(int state);
void resize_window(int w, int h);

void handleNormalKeys(unsigned char key, int x, int y);
void handleSpecialKeys(int key, int x, int y);

void handleMouseKeypress(int button, int state, int x, int y);
void handleMouseWheel(int wheel, int direction, int x, int y);
void handleMouseActiveMotion(int x, int y);
void handleMousePassiveMotion(int x, int y);

void render_bitmap_string_2d(const char * cstr, float x, float y, void * font, Color_RGBA color);
void draw_planet(GLdouble radius, GLdouble centre_x, GLdouble centre_y, Color_RGBA color);

double model_x_from_screen_x(const int& x);
double model_y_from_screen_y(const int& y);

bool is_over_menu1(const int& x) {return x <= menu1_width;}
bool is_over_menu2(const int& x) {return x > (window_width - menu2_width);}
bool is_over_scene(const int& x) {return x > menu1_width && x < (window_width - menu2_width);}

double model_x_from_screen_x(const int& x) {
    return (x - (window_width + menu1_width - menu2_width)/2 ) * model_scale;
}

double model_y_from_screen_y(const int& y) {
    return (y - window_height/2 ) * model_scale;
}

Color_RGB getRandomColor();

void onTimer(int next_timer_tick) {

    pStopWatch->start();
    for (int i = 0; i < model_speed; ++i)
        pSimpleSpace->move_one_step();
    pStopWatch->stop();
    need_to_render_scene = true;
    glutPostRedisplay();

    int corrected_period = next_timer_tick - int(pStopWatch->time_elaplsed_usec()/1000);
    if (corrected_period < 0) {
        cout << "    Warning: [onTimer] FPS degradation; calculation took: " << pStopWatch->time_elaplsed_usec()/1000 << "ms (> " << next_timer_tick << "ms)" << endl;
        corrected_period = 0;
    }

    if (simulation_on)
        glutTimerFunc(corrected_period, onTimer, next_timer_tick);
}

void check_need_to_render_bools(int next_timer_tick) {
    if (need_to_render_menu1 || need_to_render_menu2 || need_to_render_scene)
        glutPostRedisplay();
    glutTimerFunc(next_timer_tick, check_need_to_render_bools, next_timer_tick);
}

void start_simulation() {
    if (!simulation_on) {
        simulation_on = true;
        glutTimerFunc(1000/frame_rate, onTimer, 1000/frame_rate);
    }
}

void stop_simulation() {
    if (simulation_on) {
        simulation_on = false; // This stops timer cycling
    }
}

void exit() {
    glutDestroyWindow(main_window_id);
    cout << "Exiting by user choice" << endl;
    exit(0);
}

void restart_simulation() {

    bool need_to_resume = false;
    if (simulation_on) {
        simulation_on = false;
        need_to_resume = true;
    }

    pSimpleSpace->remove_all_objects();

    double dist = 4e7;
    pSimpleSpace->add_planet(Planet(Vector2d(0, 0), Vector2d(0, 0), 1e30, 3e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d( dist/4,   0), Vector2d(0,   -2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(-dist/4,   0), Vector2d(0,    2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0,  dist/1.5), Vector2d(-1.5e6, 0), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0, -dist/1.5), Vector2d( 1.5e6, 0), 1e15, 1e6, getRandomColor()));

    if (need_to_resume) {
        simulation_on = true;
    } else {
        need_to_render_scene = true;
        glutPostRedisplay();
    }
}

void remove_all_objects() {
    pSimpleSpace->remove_all_objects();
}

void zoom_in() {
    if (model_scale / 2 >= 3125) {
        model_scale /= 2;
        //cout << "zoomed out to model scale: " << model_scale << endl;
    } else {
        cout << "Max zoom in reached" << endl;
    }
}

void zoom_out() {
    if (model_scale * 2 <= 1600000) {
        model_scale *= 2;
        //cout << "zoomed in to model scale: " << model_scale << endl;
    } else {
        cout << "Max zoom out reached" << endl;
    }
}

void move_one_step() {
    if (!simulation_on) {
        pSimpleSpace->move_one_step();
        cout << "Moved for one step" << endl;
    } else {
        cout << "Stop simulation to move step by step" << endl;
    }
}

void render_window() {

    //pStopWatch_render->start();

    glEnable(GL_SCISSOR_TEST);

    // ---- Menu1 (Left) ----

    if (need_to_render_menu1) {
        need_to_render_menu1 = false;

        glViewport(0, 0, menu1_width, window_height);
        glScissor(0, 0, menu1_width, window_height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0.0f,           // left vertical clipping plane
                menu1_width,    // right vertical clipping plane
                window_height,  // bottom horizontal clipping plane
                0.0f,           // top horizontal clipping plane
                1.0f,           // nearer clipping plane
                -1.0f);         // farer clipping plane

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        pControlsLeft->draw();
    }

    // ---- Scene ----

    if (need_to_render_scene) {
        need_to_render_scene = false;

        pFPSCounter->update_on_frame();

        switch(gMode)
        {
            case ALIAS_MODE_ALIASED:
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
                glDisable(GL_MULTISAMPLE);
                break;

            case ALIAS_MODE_ANTIALIASED:
                glEnable(GL_LINE_SMOOTH);
                glEnable(GL_POLYGON_SMOOTH);
                glDisable(GL_MULTISAMPLE);
                break;

            case ALIAS_MODE_MULTISAMPLE:
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
                glEnable(GL_MULTISAMPLE);
                break;
        }

        int scene_width = window_width - (menu1_width + menu2_width);
        glViewport(menu1_width, 0, scene_width, window_height);
        glScissor(menu1_width, 0, scene_width, window_height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(menu1_width,                  // left vertical clipping plane
                window_width - menu2_width,   // right vertical clipping plane
                window_height,                // bottom horizontal clipping plane
                0.0f,                         // top horizontal clipping plane
                1.0f,                         // nearer clipping plane
                -1.0f);                       // farer clipping plane

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        int x_center_offset = (window_width + menu1_width - menu2_width)/2;
        int y_center_offset = window_height/2;

        glPushMatrix();
        glTranslated(x_center_offset, y_center_offset, 0.0);

        pSimpleSpace->draw_scene(model_scale);

        if (mouse.left_key.is_down && is_over_scene(mouse.left_key.down_x)) {
            draw_planet(next_planet.rad_m / model_scale,
                        mouse.left_key.down_x - x_center_offset,
                        mouse.left_key.down_y - y_center_offset,
                        Color_RGBA(next_planet.color, 1.0f));

            glBegin(GL_LINES);
            glColor4f(next_planet.color.R, next_planet.color.G, next_planet.color.B, 1.0f);
            glVertex2d(mouse.left_key.down_x - x_center_offset, mouse.left_key.down_y - y_center_offset);
            glVertex2d(mouse.x - x_center_offset, mouse.y - y_center_offset);
            glEnd();

            ss << next_planet.mass_kg;
            str = std::string("Mass: ") + ss.str() + std::string(" kg");
            Color_RGBA text_color = Color_RGBA(1.0f, 1.0f, 1.0f, 0.5f);
            if (mass_modifier_key_down)
                text_color.A = 1.0f;
            render_bitmap_string_2d(str.c_str(),
                                    mouse.left_key.down_x - x_center_offset - 30,
                                    mouse.left_key.down_y - y_center_offset + 25,
                                    GLUT_BITMAP_HELVETICA_12,
                                    text_color);
            ss.clear();
            ss.str(std::string());

            ss << next_planet.rad_m;
            str = std::string("Rad: ") + ss.str() + std::string(" m");
            text_color = Color_RGBA(1.0f, 1.0f, 1.0f, 0.5f);
            if (rad_modifier_key_down)
                text_color.A = 1.0f;
            render_bitmap_string_2d(str.c_str(),
                                    mouse.left_key.down_x - x_center_offset - 30,
                                    mouse.left_key.down_y - y_center_offset + 40,
                                    GLUT_BITMAP_HELVETICA_12,
                                    text_color);
            ss.clear();
            ss.str(std::string());

            text_color = Color_RGBA(1.0f, 1.0f, 1.0f, 0.5f);
            if (!mass_modifier_key_down && !rad_modifier_key_down)
                text_color.A = 1.0f;
            ss << sqrt(pow(next_planet.vel.x, 2) + pow(next_planet.vel.y, 2));
            str = std::string("Vel: ") + ss.str() + std::string(" m/s");
            render_bitmap_string_2d(str.c_str(),
                                    mouse.left_key.down_x - x_center_offset - 30,
                                    mouse.left_key.down_y - y_center_offset + 55,
                                    GLUT_BITMAP_HELVETICA_12,
                                    text_color);
            ss.clear();
            ss.str(std::string());
        }

        if (mouse.right_key.is_down && is_over_scene(mouse.right_key.down_x)) {
            glBegin(GL_LINE_LOOP);
            glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
            glVertex2d(mouse.right_key.down_x - x_center_offset,
                       mouse.right_key.down_y - y_center_offset);
            glVertex2d(mouse.x - x_center_offset,
                       mouse.right_key.down_y - y_center_offset);
            glVertex2d(mouse.x - x_center_offset,
                       mouse.y - y_center_offset);
            glVertex2d(mouse.right_key.down_x - x_center_offset,
                       mouse.y - y_center_offset);

            glEnd();
        }

        #if (BORDERS_ENABLED > 0)
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2d(RIGHT_BORDER/double(model_scale), TOP_BORDER/double(model_scale));
        glVertex2d(LEFT_BORDER/double(model_scale), TOP_BORDER/double(model_scale));
        glVertex2d(LEFT_BORDER/double(model_scale), BOTTOM_BORDER/double(model_scale));
        glVertex2d(RIGHT_BORDER/double(model_scale), BOTTOM_BORDER/double(model_scale));
        glEnd();
        #endif

        glPopMatrix();

        ss << pFPSCounter->get_fps();
        str = std::string("FPS: ") + ss.str();
        render_bitmap_string_2d(str.c_str(),
                                menu1_width + 10,
                                15,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
        ss.clear();
        ss.str(std::string());

        render_bitmap_string_2d("add/remove planets - mouse left/right keys",
                                window_width - 900,
                                window_height - 35,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));

        render_bitmap_string_2d("hold r/m to change radius or mass of new planet",
                                window_width - 900,
                                window_height - 20,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));

        render_bitmap_string_2d("a - antialiazing mode",
                                window_width - 600,
                                window_height - 35,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
        render_bitmap_string_2d("space - start/stop",
                                window_width - 600,
                                window_height - 20,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
        render_bitmap_string_2d(",/. speed",
                                window_width - 400,
                                window_height - 35,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
        render_bitmap_string_2d("q/esc - quit",
                                window_width - 400,
                                window_height - 20,
                                GLUT_BITMAP_HELVETICA_12,
                                Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));

        switch(gMode)
        {
            case ALIAS_MODE_ALIASED:
                break;

            case ALIAS_MODE_ANTIALIASED:
                glDisable(GL_LINE_SMOOTH);
                glDisable(GL_POLYGON_SMOOTH);
                break;

            case ALIAS_MODE_MULTISAMPLE:
                glDisable(GL_MULTISAMPLE);
                break;
        }

    }

    // ---- Menu2 (Right) ----

    if (need_to_render_menu2) {
        need_to_render_menu2 = false;

        glViewport(window_width - menu2_width, 0, menu2_width, window_height);
        glScissor(window_width - menu2_width, 0, menu2_width, window_height);
        glClearColor(0.3f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(window_width - menu2_width,  // left vertical clipping plane
                window_width,                // right vertical clipping plane
                window_height,               // bottom horizontal clipping plane
                0.0f,                        // top horizontal clipping plane
                1.0f,                        // nearer clipping plane
                -1.0f);                      // farer clipping plane

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        pControlsRight->draw();
    }

    glDisable(GL_SCISSOR_TEST);
    glutSwapBuffers();

    //pStopWatch_render->stop();
    //cout << "Frame rendering took " << pStopWatch_render->time_elaplsed_usec() << "us" << endl;
}

void handle_window_visibility(int state) {
    notify_to_update_all();
    glutPostRedisplay();
}

void resize_window(int w, int h) {

    pControlsRight->shift_conrols_position(w - window_width, 0);

    window_width = w;
    window_height = h;

    notify_to_update_all();
    glutPostRedisplay();
}

void handleNormalKeysDown(unsigned char key, int x, int y) {

    // To see if modifier key is pressed use: (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    pControlsLeft->handle_keyboard_key_event(key, KEY_DOWN);
    pControlsRight->handle_keyboard_key_event(key, KEY_DOWN);

    switch (key)
    {
        // Exit
        case 27:  // Escape key
        case 'q':
            exit();
            break; // Never get to this line

        // Antialiazing mode
        case 'a':
            switch(gMode)
            {
                case ALIAS_MODE_ALIASED:
                    cout << "Antialiased" << endl;
                    gMode = ALIAS_MODE_ANTIALIASED;
                    break;

                case ALIAS_MODE_ANTIALIASED:
                    cout << "Multisampled" << endl;
                    gMode = ALIAS_MODE_MULTISAMPLE;
                    break;

                case ALIAS_MODE_MULTISAMPLE:
                    cout << "Aliased" << endl;
                    gMode = ALIAS_MODE_ALIASED;
                    break;
            }
            break;

        // Speed
        case ',':
            if (model_speed > 1) {
                model_speed /= 10;
                cout << "model speed: " << model_speed << " (" << frame_rate * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
            }
            break;

        case '.':
            if (!(model_speed * pSimpleSpace->get_model_time_step_ms() > 100000)) {
                model_speed *= 10;
                cout << "model speed: " << model_speed << " (" << frame_rate * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
            }
            break;
        case 'r':
        case 'R':
            rad_modifier_key_down = true;
            break;
        case 'm':
        case 'M':
            mass_modifier_key_down = true;
            break;
    }

    notify_to_update_all();
    glutPostRedisplay();
}

void handleNormalKeysUp(unsigned char key, int x, int y) {

    pControlsLeft->handle_keyboard_key_event(key, KEY_UP);
    pControlsRight->handle_keyboard_key_event(key, KEY_UP);

    switch (key)
    {
        case 'r':
        case 'R':
            rad_modifier_key_down = false;
            break;
        case 'm':
        case 'M':
            mass_modifier_key_down = false;
            break;
    }

    notify_to_update_all();
    glutPostRedisplay();
}

void handleSpecialKeysDown(int key, int x, int y) {

    switch (key)
    {
        case GLUT_KEY_LEFT:
            pControlsLeft->handle_keyboard_key_event(ARROW_LEFT, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(ARROW_LEFT, KEY_DOWN);
            break;

        case GLUT_KEY_RIGHT:
            pControlsLeft->handle_keyboard_key_event(ARROW_RIGHT, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(ARROW_RIGHT, KEY_DOWN);
            break;

        case GLUT_KEY_UP:
            pControlsLeft->handle_keyboard_key_event(ARROW_UP, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(ARROW_UP, KEY_DOWN);
            break;

        case GLUT_KEY_DOWN:
            pControlsLeft->handle_keyboard_key_event(ARROW_DOWN, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(ARROW_DOWN, KEY_DOWN);
            break;

        case GLUT_KEY_HOME:
            pControlsLeft->handle_keyboard_key_event(HOME_KEY, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(HOME_KEY, KEY_DOWN);
            break;

        case GLUT_KEY_END:
            pControlsLeft->handle_keyboard_key_event(END_KEY, KEY_DOWN);
            pControlsRight->handle_keyboard_key_event(END_KEY, KEY_DOWN);
            break;
    }

    notify_to_update_all();
    glutPostRedisplay();
}

// Not used (no need)
void handleSpecialKeysUp(int key, int x, int y) {cout << "handleSpecialKeysUp" << endl;}

void handleMouseKeypress(int button, int state, int x, int y) {

    MOUSE_KEY current_mouse_key;
    KEY_ACTION current_mouse_key_action;

    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            mouse.left_key.update((state == GLUT_DOWN), x, y);
            current_mouse_key = MOUSE_LEFT_KEY;
            break;
        case GLUT_MIDDLE_BUTTON:
            mouse.middle_key.update((state == GLUT_DOWN), x, y);
            current_mouse_key = MOUSE_MIDDLE_KEY;
            break;
        case GLUT_RIGHT_BUTTON:
            mouse.right_key.update((state == GLUT_DOWN), x, y);
            current_mouse_key = MOUSE_RIGHT_KEY;
            break;
    }

    switch (state)
    {
        case GLUT_DOWN:
            current_mouse_key_action = KEY_DOWN;
            break;

        case GLUT_UP:
            current_mouse_key_action = KEY_UP;
            break;
    }

    pControlsLeft->handle_mouse_key_event(mouse, current_mouse_key, current_mouse_key_action);
    pControlsRight->handle_mouse_key_event(mouse, current_mouse_key, current_mouse_key_action);

    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            switch (state)
            {
                case GLUT_DOWN: // Prepare planet to be added
                    if (is_over_scene(x)) {

                        double new_mass = 1e29;
                        Slider* mass_slider = dynamic_cast<Slider *>(pControlsLeft->find_by_id(mass_slider_id));
                        if (mass_slider != NULL)
                            new_mass = mass_slider->get_value();
                        else
                            cout << "mass slider not found" << endl;

                        double new_rad = 2e6;
                        Slider* rad_slider = dynamic_cast<Slider *>(pControlsLeft->find_by_id(radius_slider_id));
                        if (rad_slider != NULL)
                            new_rad = rad_slider->get_value();
                        else
                            cout << "rad slider not found" << endl;

                        next_planet.reset_parameters();
                        next_planet.pos.x = next_planet.prev_pos.x = (x - (window_width + menu1_width - menu2_width)/2) * model_scale;
                        next_planet.pos.y = next_planet.prev_pos.y = (y - window_height/2) * model_scale;
                        next_planet.mass_kg = new_mass;
                        next_planet.rad_m = new_rad;
                        next_planet.color = getRandomColor();
                    }

                    break;
                case GLUT_UP: // Add prepared planet
                    if (is_over_scene(mouse.left_key.down_x)) {
                        pSimpleSpace->add_planet(next_planet);
                    }
                    break;
            }
            break;

        case GLUT_RIGHT_BUTTON:
            switch (state)
            {
                case GLUT_DOWN: // Immediately remove planet(s) at pressed position
                {
                    if (is_over_scene(x)) {
                        Physics::Vector2d clicked_model_pos(model_x_from_screen_x(mouse.x),
                                                            model_y_from_screen_y(mouse.y));
                        pair<bool, unsigned int> ret = pSimpleSpace->find_planet_by_click(clicked_model_pos);
                        if (ret.first) {
                            pSimpleSpace->remove_planet(ret.second);
                        }
                    }
                    break;
                }
                case GLUT_UP:
                {
                    if (is_over_scene(mouse.right_key.down_x) &&
                        (x - mouse.right_key.down_x) != 0  &&
                        (y - mouse.right_key.down_y) != 0) {

                        Physics::Vector2d sel_start_model_pos(model_x_from_screen_x(mouse.right_key.down_x),
                                                              model_y_from_screen_y(mouse.right_key.down_y));
                        Physics::Vector2d sel_end_model_pos(model_x_from_screen_x(mouse.x),
                                                            model_y_from_screen_y(mouse.y));
                        std::vector<unsigned int> id_list = pSimpleSpace->find_planets_by_selection(sel_start_model_pos, sel_end_model_pos);
                        for (std::vector<unsigned int>::iterator it = id_list.begin(), it_end = id_list.end(); it != it_end; ++it) {
                            pSimpleSpace->remove_planet(*it);
                        }
                    }
                    break;
                }
            }
            break;
    }

    notify_to_update_all();
    glutPostRedisplay();
}

// Not used (because not working on default Mac OS X GLUT)
void handleMouseWheel(int wheel, int direction, int x, int y) {
    cout << "wheel: " << wheel << " direction: " << direction << endl;
}

void handleMouseActiveMotion(int x, int y) {

    mouse.x = x;
    mouse.y = y;

    pControlsLeft->handle_mouse_move(mouse);
    pControlsRight->handle_mouse_move(mouse);

    if (mouse.left_key.is_down) {
        if (mass_modifier_key_down) {
            next_planet.mass_kg = sqrt(pow((x - mouse.left_key.down_x), 2) + \
                                       pow((y - mouse.left_key.down_y), 2)) * 1e30;
        } else if (rad_modifier_key_down) {
            next_planet.rad_m = sqrt(pow((x - mouse.left_key.down_x) * model_scale, 2) + \
                                     pow((y - mouse.left_key.down_y) * model_scale, 2));
        } else {
            next_planet.vel.x = (x - mouse.left_key.down_x) * model_scale;
            next_planet.vel.y = (y - mouse.left_key.down_y) * model_scale;
        }
    }

    notify_to_update_all();
    glutPostRedisplay();
}

void handleMousePassiveMotion(int x, int y) {

    mouse.x = x;
    mouse.y = y;

    pControlsLeft->handle_mouse_move(mouse);
    pControlsRight->handle_mouse_move(mouse);

    notify_to_update_all();
    glutPostRedisplay();
}

// Not used, as consumes 100% CPU
void idle_callback() {
    if (need_to_render_menu1 || need_to_render_menu2 || need_to_render_scene)
        glutPostRedisplay();
}

// Not used (no need)
void overlay_callback() {cout << "overlay_callback" << endl;}

// Render 2D text
void render_bitmap_string_2d(const char * cstr, float x, float y, void * font, Color_RGBA color) {
    const char * ch;
    glColor4f(color.R, color.G, color.B, color.A);
    glRasterPos2f(x, y);
    for (ch = cstr; *ch != '\0'; ch++)
        glutBitmapCharacter(font, *ch);
}

//Draw a 2D painted cicle using GL_TRIANGLE_FAN
void draw_planet(GLdouble rad, GLdouble centre_x, GLdouble centre_y, Color_RGBA color)
{
    if (rad < 1)
        rad = 1;

    glColor4f(color.R, color.G, color.B, color.A);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(centre_x, centre_y);
    double delta = M_PI / 100;
    for (double a = 0; a <= 2 * M_PI; a += delta)
        glVertex2d(rad * cos(a) + centre_x, rad * sin(a) + centre_y);
    glEnd();
}

Color_RGB getRandomColor() {
    return Color_RGB(static_cast<float>((rand()%10 + 1)/10.0),
            static_cast<float>((rand()%10 + 1)/10.0),
            static_cast<float>((rand()%10 + 1)/10.0));
}

//Initializes rendering
void initRendering() {

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}


// default changed to make glutInit() work
//int main(int argc, const char * argv[])
int main(int argc, char * argv[])
{
    // Seed for random values
    srand(static_cast<unsigned int>(time(NULL)));

    // SimpleSpace testing begin
    double dist = 4e7;
    pSimpleSpace->add_planet(Planet(Vector2d(0, 0), Vector2d(0, 0), 1e30, 3e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d( dist/4,   0), Vector2d(0,   -2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(-dist/4,   0), Vector2d(0,    2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0,  dist/1.5), Vector2d(-1.5e6, 0), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0, -dist/1.5), Vector2d( 1.5e6, 0), 1e15, 1e6, getRandomColor()));

    pControlsLeft->add_button_boolean(20, 20,           // x, y
                                      160, 30,           // w, h
                                       "Simulation On",   // Label
                                       simulation_on,     // Initial state
                                       start_simulation,  // Callback On
                                       stop_simulation);  // Callback Off

    pControlsLeft->add_button(20, 60,               // x, y
                              160, 30,              // w, h
                              "Move One Step",      // Label
                               move_one_step);       // Callback

    pControlsLeft->add_button(20, 100,              // x, y
                              75, 30,               // w, h
                              "Restart",            // Label
                              restart_simulation);  // Callback

    pControlsLeft->add_button(105, 100,             // x, y
                              75, 30,               // w, h
                              "Clear All",          // Label
                               remove_all_objects);  // Callback

    pControlsLeft->add_button(40, 140,      // x, y
                              55, 30,       // w, h
                              "+",          // Label
                              zoom_in);     // Callback

    pControlsLeft->add_button(105, 140,     // x, y
                              55, 30,       // w, h
                              "-",          // Label
                              zoom_out);    // Callback
    radius_slider_id = \
    pControlsLeft->add_slider(20, 280,      // x, y
                              160, 60,      // w, h
                              1e6, 3e6,     // Min, Max
                              2e6,          // Value
                              "Radius");    // Label

    mass_slider_id = \
    pControlsLeft->add_slider(20, 350,      // x, y
                              160, 60,      // w, h
                              5e28, 1e30,   // Min, Max
                              1e29,         // Value
                              "Mass");      // Label

    pControlsLeft->add_slider(20, 420,      // x, y
                              160, 60,      // w, h
                              -100, 450,    // Min, Max
                              30,           // Value
                              "Test");      // Label

    pControlsLeft->add_numeric_box(30, 490, // x, y
                                   140, 20, // w, h
                                   123);     // Value

    pControlsLeft->add_button(40, 540,      // x, y
                              120, 30,      // w, h
                              "Exit",       // Label
                              exit);        // Callback

    pControlsLeft->add_redraw_box(10, 580,  // x, y
                                10, 10);    // w, h

    pControlsRight->add_numeric_box(window_width - menu2_width + 10, 10, // x, y
                                    80, 20, // w, h
                                    123);   // Value

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(window_width, window_height);
    main_window_id = glutCreateWindow("Simple Space");
    initRendering();

    // Render & Resize
    glutDisplayFunc(render_window);
    glutVisibilityFunc(handle_window_visibility);
    glutReshapeFunc(resize_window);

    // Keyboard
    //glutIgnoreKeyRepeat(1); // Don't use glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF), it disables globally (for other apps)
    glutKeyboardFunc(handleNormalKeysDown);
    glutKeyboardUpFunc(handleNormalKeysUp);
    glutSpecialFunc(handleSpecialKeysDown);
    //glutSpecialUpFunc(handleSpecialKeysUp); // Not used (no need)

    // Mouse
    glutMouseFunc(handleMouseKeypress);
    //glutMouseWheelFunc(handleMouseWheel); // Not used (because not working on default Mac OS X GLUT)
    glutMotionFunc(handleMouseActiveMotion);
    glutPassiveMotionFunc(handleMousePassiveMotion);

    // Timers
    if (simulation_on)
        glutTimerFunc(1000/frame_rate, onTimer, 1000/frame_rate);
    glutTimerFunc(100, check_need_to_render_bools, 100);

    // Other callbacks
    //glutIdleFunc(idle_callback); // Not used (consumes 100% CPU)
    //glutOverlayDisplayFunc(overlay_callback);// Not used (no need)

    /*
    // FreeType library initialization
    int ft_error = FT_Init_FreeType(&ft_library);
    if (ft_error != 0) {
        cout << "Error occurred during library initialization; code: " << ft_error << endl;
    }

    ft_error = FT_New_Face(ft_library,
                         "/Library/Fonts/Microsoft/Calibri.ttf",
                         0,
                         &face );
    if (ft_error == FT_Err_Unknown_File_Format) {
        cout << "Error: the font format seems to be unsupported; code: " << ft_error << endl;
    } else if (ft_error) {
        cout << "Error: the font file couldn't been opened and read; code: " << ft_error << endl;
    }

    ft_error = FT_Set_Char_Size(face,    // handle to face object
                                0,       // char_width in 1/64th of points
                                16*64,   // char_height in 1/64th of points
                                300,     // horizontal device resolution
                                300 );   // vertical device resolution
    */

    glutMainLoop();

    // Never get here
    return 0;
}

// Known bugs:
// 1. System constantly gains energy during long continious collision
