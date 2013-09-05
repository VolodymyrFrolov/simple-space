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

#if defined(ASD_ASD)
#endif

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
#include <time.h>   // time()

#include "controlsmanager.h"
#include "simplespace.h"
#include "planet.h"
#include "physics.h"

//FT_Library  ft_library; // FreeType library handler
//FT_Face     face;       // Face object handler

int main_window;
int start_stop_button = -1;

const int FRAMERATE = 60;
int window_width = 0;
int window_height = 0;

const int menu_width = 200;
const int menu_min_height = 400;
const int scene_min_width = 100;

int model_speed = 1;
int model_scale = 200000;

bool simulation_on = true;

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

// Creating global SimpleSpace
std::unique_ptr<SimpleSpace> pSimpleSpace(new SimpleSpace(1000/FRAMERATE));
std::unique_ptr<ControlsManager> pControls(new ControlsManager);

void initRendering();
void onTimer(int next_timer_tick);

void drawScene();
void resizeWindow(int w, int h);

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

double model_x_from_screen_x(const int& x) {
    return (x - (window_width + menu_width)/2 ) * model_scale;
}

double model_y_from_screen_y(const int& y) {
    return (y - window_height/2 ) * model_scale;
}

Color_RGB getRandomColor();

void onTimer(int next_timer_tick) {

    for (int i = 0; i < model_speed; ++i)
        pSimpleSpace->move_one_step();
    glutPostRedisplay();

    if (simulation_on)
        glutTimerFunc(next_timer_tick, onTimer, next_timer_tick);
}

void start_simulation() {
    if (!simulation_on) {
        simulation_on = true;
        glutTimerFunc(1000/FRAMERATE, onTimer, 1000/FRAMERATE);
    }
}

void stop_simulation() {
    if (simulation_on) {
        simulation_on = false; // This stops timer cycling
    }
}

void exit() {
    glutDestroyWindow(main_window);
    cout << "Exiting by user choice" << endl;
    exit(0);
}

void restart_simulation() {
    cout << "Restart pressed" << endl;

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

    if (need_to_resume)
        simulation_on = true;
    else
        glutPostRedisplay();
}

void zoom_in() {
    if (model_scale / 2 >= 3125) {
        model_scale /= 2;
        //cout << "zoomed out to model scale: " << model_scale << endl;
    } else {
        cout << "Can't zoom in any more" << endl;
    }
}

void zoom_out() {
    if (model_scale * 2 <= 1600000) {
        model_scale *= 2;
        //cout << "zoomed in to model scale: " << model_scale << endl;
    } else {
        cout << "Can't zoom out any more" << endl;
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

void drawScene() {

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

    // ---- Menu ----
    glViewport(0, 0, menu_width, window_height);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glScissor(0, 0, menu_width, window_height);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0.0f,           // left vertical clipping plane
            menu_width,     // right vertical clipping plane
            window_height,  // bottom horizontal clipping plane
            0.0f,           // top horizontal clipping plane
            1.0f,           // nearer clipping plane
            -1.0f);         // farer clipping plane

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    pControls->draw();

    glDisable(GL_SCISSOR_TEST);

    // ---- Scene ----
    int scene_width = window_width - menu_width > 1 ? window_width - menu_width : 1;
    glViewport(menu_width, 0, scene_width, window_height);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glScissor(menu_width, 0, scene_width, window_height);
    glEnable(GL_SCISSOR_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(menu_width,     // left vertical clipping plane
            window_width,   // right vertical clipping plane
            window_height,  // bottom horizontal clipping plane
            0.0f,           // top horizontal clipping plane
            1.0f,           // nearer clipping plane
            -1.0f);         // farer clipping plane

    glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();

    glPushMatrix();
    glTranslated((window_width + menu_width)/2.0, window_height/2.0, 0.0);

    //glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f); //Rotate the camera
    //glPushMatrix(); //Save the transformations performed thus far
    //glPopMatrix(); //Undo the move to the center
    
    for (std::vector<Planet>::const_iterator it = pSimpleSpace->planets.begin(),
        it_end = pSimpleSpace->planets.end(); it != it_end; ++it) {
        draw_planet(it->rad_m/model_scale, it->pos.x/model_scale, it->pos.y/model_scale, {it->color.R, it->color.G, it->color.B, 1.0f});
    }

    if (mouse.left_key.is_down && (mouse.left_key.down_x > menu_width)) {
        draw_planet(next_planet.rad_m / model_scale,
                    mouse.left_key.down_x - (window_width + menu_width)/2.0,
                    mouse.left_key.down_y - window_height/2.0,
                    Color_RGBA(next_planet.color, 1.0f));

        glBegin(GL_LINES);
        glColor4f(next_planet.color.R, next_planet.color.G, next_planet.color.B, 1.0f);
        glVertex2d(mouse.left_key.down_x - (window_width + menu_width)/2.0, mouse.left_key.down_y - window_height/2.0);
        glVertex2d(mouse.x - (window_width + menu_width)/2.0, mouse.y - window_height/2.0);
        glEnd();

        ss << next_planet.mass_kg;
        str = std::string("Mass: ") + ss.str() + std::string(" kg");
        Color_RGBA text_color = Color_RGBA(1.0f, 1.0f, 1.0f, 0.5f);
        if (mass_modifier_key_down)
            text_color.A = 1.0f;
        render_bitmap_string_2d(str.c_str(),
                                mouse.left_key.down_x - (window_width + menu_width)/2.0 - 30,
                                mouse.left_key.down_y - window_height/2.0 + 25,
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
                                mouse.left_key.down_x - (window_width + menu_width)/2.0 - 30,
                                mouse.left_key.down_y - window_height/2.0 + 40,
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
                                mouse.left_key.down_x - (window_width + menu_width)/2.0 - 30,
                                mouse.left_key.down_y - window_height/2.0 + 55,
                                GLUT_BITMAP_HELVETICA_12,
                                text_color);
        ss.clear();
        ss.str(std::string());
    }

    if (mouse.right_key.is_down && (mouse.right_key.down_x > menu_width)) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex2d(mouse.right_key.down_x - (window_width + menu_width)/2.0,
                   mouse.right_key.down_y - window_height/2.0);
        glVertex2d(mouse.x - (window_width + menu_width)/2.0,
                   mouse.right_key.down_y - window_height/2.0);
        glVertex2d(mouse.x - (window_width + menu_width)/2.0,
                   mouse.y - window_height/2.0);
        glVertex2d(mouse.right_key.down_x - (window_width + menu_width)/2.0,
                   mouse.y - window_height/2.0);

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
        
    render_bitmap_string_2d("add/remove planets - mouse left/right keys",
                            window_width - 800,
                            window_height - 35,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    
    render_bitmap_string_2d("hold r/m to change radius or mass of new planet",
                            window_width - 800,
                            window_height - 20,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));

    render_bitmap_string_2d("a - antialiazing mode",
                            window_width - 400,
                            window_height - 35,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    render_bitmap_string_2d("c - clear all planets",
                            window_width - 400,
                            window_height - 20,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    render_bitmap_string_2d("space - start/stop",
                            window_width - 400,
                            window_height - 5,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    render_bitmap_string_2d("+/- zoom",
                            window_width - 100,
                            window_height - 35,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    render_bitmap_string_2d(",/. speed",
                            window_width - 100,
                            window_height - 20,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));
    render_bitmap_string_2d("q/esc - quit",
                            window_width - 100,
                            window_height - 5,
                            GLUT_BITMAP_HELVETICA_12,
                            Color_RGBA(0.9f, 0.9f, 0.9f, 1.0f));

    switch(gMode)
    {
        case ALIAS_MODE_ANTIALIASED:
            glDisable(GL_LINE_SMOOTH);
            glDisable(GL_POLYGON_SMOOTH);
            break;

        case ALIAS_MODE_MULTISAMPLE:
            glDisable(GL_MULTISAMPLE);
            break;

        case ALIAS_MODE_ALIASED:
            break;
    }

    glutSwapBuffers();
}

void resizeWindow(int w, int h) {
    window_width = w;
    window_height = h;
}

//Called when a key is pressed
void handleNormalKeysDown(unsigned char key, int x, int y) {
    // To see if modifier key is pressed use:
    // (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    
    //cout << "key:" << key << endl;

    switch (key)
    {
        // Remove objects
        case 'c':
            pSimpleSpace->remove_all_objects();
            break;

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

        case ' ':
            pControls->simulate_mouse_action(start_stop_button, MOUSE_LEFT_KEY, MOUSE_KEY_DOWN);
            break;

        // Zoom
        case '-':
            zoom_out();
            break;

        case '=':
            zoom_in();
            break;

        // Speed
        case ',':
            if (model_speed > 1) {
                model_speed /= 10;
                cout << "model speed: " << model_speed << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
            }
            break;

        case '.':
            if (!(model_speed * pSimpleSpace->get_model_time_step_ms() > 100000)) {
                model_speed *= 10;
                cout << "model speed: " << model_speed << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
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

    if (!simulation_on) glutPostRedisplay();
}

void handleNormalKeysUp(unsigned char key, int x, int y) {
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
        case ' ':
            pControls->simulate_mouse_action(start_stop_button, MOUSE_LEFT_KEY, MOUSE_KEY_UP);
            break;
    }

    if (!simulation_on) glutPostRedisplay();
}

// Not used
void handleSpecialKeysDown(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_RIGHT:
            break;

        case GLUT_KEY_UP:
            break;

        case GLUT_KEY_LEFT:
            break;

        case GLUT_KEY_DOWN:
            break;
    }

    //if (!simulation_on) glutPostRedisplay();
}

void handleMouseKeypress(int button, int state, int x, int y) {
    //cout << "Button: " << button << " state: " << state << endl;

    // Update global mouse; current mouse-key and it's action

    MOUSE_KEY current_mouse_key;
    MOUSE_KEY_ACTION current_mouse_key_action;

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
            current_mouse_key_action = MOUSE_KEY_DOWN;
            break;

        case GLUT_UP:
            current_mouse_key_action = MOUSE_KEY_UP;
            break;
    }

    pControls->handle_mouse_key_event(mouse, current_mouse_key, current_mouse_key_action);

    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            switch (state)
            {
                case GLUT_DOWN: // Prepare planet to be added
                    if (x > menu_width) {
                        next_planet.reset_parameters();
                        next_planet.pos.x = next_planet.prev_pos.x = (x - (window_width + menu_width)/2) * model_scale;
                        next_planet.pos.y = next_planet.prev_pos.y = (y - window_height/2) * model_scale;
                        next_planet.mass_kg = 1e29;
                        next_planet.rad_m = 2e6;
                        next_planet.color = getRandomColor();
                    }

                    break;
                case GLUT_UP: // Add prepared planet
                    if (mouse.left_key.down_x > menu_width) {
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
                    if (x > menu_width) {
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
                    if (mouse.right_key.down_x > menu_width &&
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

    if (!simulation_on) glutPostRedisplay();
}

void handleMouseWheel(int wheel, int direction, int x, int y) {
    cout << "wheel: " << wheel << " direction: " << direction << endl;
}

// Mouse motion while some keys are being pressed
void handleMouseActiveMotion(int x, int y) {

    mouse.x = x;
    mouse.y = y;
    pControls->handle_mouse_move(mouse);

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

    if (!simulation_on) glutPostRedisplay();
}

// Mouse motion without keys being pressed
void handleMousePassiveMotion(int x, int y) {
    mouse.x = x;
    mouse.y = y;
    pControls->handle_mouse_move(mouse);
    if (!simulation_on) glutPostRedisplay();
}

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
    return {static_cast<float>((rand()%10 + 1)/10.0),
            static_cast<float>((rand()%10 + 1)/10.0),
            static_cast<float>((rand()%10 + 1)/10.0)};
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

    start_stop_button = \
    pControls->add_button_on_off(40, 80,            // x, y
                                 120, 30,           // w, h
                                 "Simulation On",   // Label
                                 true,              // Initial state
                                 start_simulation,  // Callback On
                                 stop_simulation);  // Callback Off

    pControls->add_button(40, 120,              // x, y
                          120, 30,              // w, h
                          "Move One Step",      // Label
                          move_one_step);       // Callback

    pControls->add_button(40, 200,              // x, y
                          120, 30,              // w, h
                          "Restart",            // Label
                          restart_simulation);  // Callback

    pControls->add_button(40, 240,      // x, y
                          55, 30,       // w, h
                          "+",          // Label
                          zoom_in);     // Callback

    pControls->add_button(105, 240,     // x, y
                          55, 30,       // w, h
                          "-",          // Label
                          zoom_out);    // Callback
    
    pControls->add_slider(20, 280,      // x, y
                          160, 60,      // w, h
                          1, 1e30,      // Min, Max
                          1e29,         // Value
                          "Radiurs: "); // Label

    pControls->add_button(40, 500,              // x, y
                          120, 30,              // w, h
                          "Exit",               // Label
                          exit);                // Callback

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1024, 600);
    main_window = glutCreateWindow("Simple Space");
    initRendering();

    // Render & Resize
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resizeWindow);

    // Keyboard
    glutIgnoreKeyRepeat(1); // Don't use glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF), it disables globally (for other apps)
    glutKeyboardFunc(handleNormalKeysDown);
    glutKeyboardUpFunc(handleNormalKeysUp);
    //glutSpecialFunc(handleSpecialKeysDown);

    // Mouse
    glutMouseFunc(handleMouseKeypress);
    //glutMouseWheelFunc(handleMouseWheel);
    glutMotionFunc(handleMouseActiveMotion);
    glutPassiveMotionFunc(handleMousePassiveMotion);

    // Timer
    glutTimerFunc(1000/FRAMERATE, onTimer, 1000/FRAMERATE);

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
