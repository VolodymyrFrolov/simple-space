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

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <GLUT/glut.h>
#elif __linux__
  //#include <GL/glut.h>
    #include <GL/freeglut.h>
#else
    // Unsupproted platform
#endif

#include <stdlib.h> // For rand()
#include <time.h>   // For time()

#include "controlsmanager.h"
#include "simplespace.h"
#include "planet.h"
#include "physics.h"

const int FRAMERATE = 60;
int model_speed = 1;
int model_scale = 200000;
int view_offset_x = 0;
int view_offset_y = 0;

Mouse mouse = {0,0, false,0,0, false,0,0};

ControlsManager controls;

void button_func() {
    cout << "button pressed" << endl;
}

bool mouse_left_key_is_down = false;
int mouse_left_key_down_x = 0;
int mouse_left_key_down_y = 0;

bool mouse_right_key_is_down = false;
int mouse_right_key_down_x = 0;
int mouse_right_key_down_y = 0;

int mouse_active_motion_x = 0;
int mouse_active_motion_y = 0;

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
SimpleSpace * pSimpleSpace = new SimpleSpace(1000/FRAMERATE);

void initRendering();
void update(int value);

void drawScene();
void handleResize(int w, int h);

void handleNormalKeys(unsigned char key, int x, int y);
void handleSpecialKeys(int key, int x, int y);

void handleMouseKeypress(int button, int state, int x, int y);
void handleMouseActiveMotion(int x, int y);
void handleMousePassiveMotion(int x, int y);

void render_bitmap_string_2d(const char * cstr, float x, float y, void * font, Color_RGBA color);
void draw_planet(GLdouble radius, GLdouble centre_x, GLdouble centre_y, Color_RGBA color);

Color_RGB getRandomColor();

//Initializes rendering
void initRendering()
{
    glEnable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glDisable( GL_LINE_SMOOTH );
    glDisable( GL_POLYGON_SMOOTH );
    glDisable( GL_MULTISAMPLE );
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, window_width, window_height);
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    
    glOrtho(0.0f,                // left vertical clipping plane
            window_width,        // right vertical clipping plane
            window_height,       // bottom horizontal clipping plane
            0.0f,                // top horizontal clipping plane
            1.0f,                // nearer clipping plane
            -1.0f);              // farer clipping plane
    
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}

void update(int value)
{
    for (int i = 0; i < model_speed; ++i) {
        pSimpleSpace->move_one_step();
    }
    glutPostRedisplay(); //Tell GLUT that the display has changed
    glutTimerFunc(1000/FRAMERATE, update, 0);
}

//Draws the 3D scene
void drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);


    glPushMatrix();
    glTranslated(window_width/2.0 + view_offset_x, window_height/2.0 + view_offset_y, 0.0);

    //glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f); //Rotate the camera
    //glPushMatrix(); //Save the transformations performed thus far
    //glPopMatrix(); //Undo the move to the center
    
    for (std::vector<Planet>::const_iterator it = pSimpleSpace->planets.begin(),
        it_end = pSimpleSpace->planets.end(); it != it_end; ++it) {
        draw_planet(it->rad_m/model_scale, it->pos.x/model_scale, it->pos.y/model_scale, {it->color.R, it->color.G, it->color.B, 1.0f});
    }

    if (mouse_left_key_is_down) {
        draw_planet(next_planet.rad_m / model_scale,
                    mouse_left_key_down_x - window_width/2.0,
                    mouse_left_key_down_y - window_height/2.0,
                    Color_RGBA(next_planet.color, 1.0f));

        glBegin(GL_LINES);
        glColor4f(next_planet.color.R, next_planet.color.G, next_planet.color.B, 1.0f);
        glVertex2d(mouse_left_key_down_x - window_width/2.0, mouse_left_key_down_y - window_height/2.0);
        glVertex2d(mouse_active_motion_x - window_width/2.0, mouse_active_motion_y - window_height/2.0);
        glEnd();

        ss << next_planet.mass_kg;
        str = std::string("Mass: ") + ss.str() + std::string(" kg");
        Color_RGBA text_color = Color_RGBA(1.0f, 1.0f, 1.0f, 0.5f);
        if (mass_modifier_key_down)
            text_color.A = 1.0f;
        render_bitmap_string_2d(str.c_str(),
                                mouse_left_key_down_x - window_width/2.0 - 30,
                                mouse_left_key_down_y - window_height/2.0 + 25,
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
                                mouse_left_key_down_x - window_width/2.0 - 30,
                                mouse_left_key_down_y - window_height/2.0 + 40,
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
                                mouse_left_key_down_x - window_width/2.0 - 30,
                                mouse_left_key_down_y - window_height/2.0 + 55,
                                GLUT_BITMAP_HELVETICA_12,
                                text_color);
        ss.clear();
        ss.str(std::string());
    }
    
    if (mouse_right_key_is_down) {
        glBegin(GL_LINE_LOOP);
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
        glVertex2d(mouse_right_key_down_x - window_width/2.0,
                   mouse_right_key_down_y - window_height/2.0);
        glVertex2d(mouse_active_motion_x - window_width/2.0,
                   mouse_right_key_down_y - window_height/2.0);
        glVertex2d(mouse_active_motion_x - window_width/2.0,
                   mouse_active_motion_y - window_height/2.0);
        glVertex2d(mouse_right_key_down_x - window_width/2.0,
                   mouse_active_motion_y - window_height/2.0);

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
    controls.draw_buttons();

    switch(gMode)
    {
        case ALIAS_MODE_ANTIALIASED:
            glDisable( GL_LINE_SMOOTH );
            glDisable( GL_POLYGON_SMOOTH );
            break;

        case ALIAS_MODE_MULTISAMPLE:
            glDisable( GL_MULTISAMPLE );
            break;

        case ALIAS_MODE_ALIASED:
            break;
    }

    glutSwapBuffers();
}

//Called when the window is resized
void handleResize(int w, int h)
{
    glViewport(0, 0, w, h);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    glOrtho(0.0f,               // left vertical clipping plane
            w,                  // right vertical clipping plane
            h,                  // bottom horizontal clipping plane
            0.0f,               // top horizontal clipping plane
            1.0f,               // nearer clipping plane
            -1.0f);             // farer clipping plane
}

//Called when a key is pressed
void handleNormalKeysDown(unsigned char key, int x, int y) {
    // To see if modifier key is pressed use:
    // (glutGetModifiers() & GLUT_ACTIVE_SHIFT)
    switch (key)
    {
        // Remove objects
        case 'c':
            pSimpleSpace->remove_all_objects();
            break;

        // Exit
        case 27:  // Escape key
        case 'q':
            delete pSimpleSpace;
            pSimpleSpace = NULL;
            cout << "handleKeypress(): exit(0)" << endl;
            exit(0);

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

        // Zoom
        case '-':
            model_scale *= 2;
            cout << "model scale: " << model_scale << endl;
            break;

        case '=':
            model_scale /= 2;
            cout << "model scale: " << model_scale << endl;
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
    }
}

void handleSpecialKeysDown(int key, int x, int y) {
    switch (key)
    {
        case GLUT_KEY_RIGHT:
            view_offset_x -= 50;
            break;

        case GLUT_KEY_UP:
            view_offset_y += 50;
            break;

        case GLUT_KEY_LEFT:
            view_offset_x += 50;
            break;

        case GLUT_KEY_DOWN:
            view_offset_y -= 50;
            break;
    }
}

void handleMouseKeypress(int button, int state, int x, int y) {

    // Update global mouse buttons status booleans and positions
    if (button == GLUT_LEFT_BUTTON) {
        mouse_left_key_is_down = (state == GLUT_DOWN);
        if (mouse_left_key_is_down) {
            mouse_left_key_down_x = x;
            mouse_left_key_down_y = y;
        }
    } else if (button == GLUT_RIGHT_BUTTON) {
        mouse_right_key_is_down = (state == GLUT_DOWN);
        if (mouse_right_key_is_down) {
           mouse_right_key_down_x = x;
           mouse_right_key_down_y = y;
        }
    }

    // Save global initial active start position of mouse
    mouse_active_motion_x = x;
    mouse_active_motion_y = y;

    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    // Perform actions on buttons clics
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            switch (state)
            {
                case GLUT_DOWN: // Prepare planet to be added
                    next_planet.reset_parameters();
                    next_planet.pos.x = next_planet.prev_pos.x = (x - window_width/2) * model_scale;
                    next_planet.pos.y = next_planet.prev_pos.y = (y - window_height/2) * model_scale;
                    next_planet.mass_kg = 1e29;
                    next_planet.rad_m = 2e6;
                    next_planet.color = getRandomColor();
                    break;
                case GLUT_UP: // Add prepared planet
                    pSimpleSpace->add_planet(next_planet);
                    break;
            }
            break;

        case GLUT_RIGHT_BUTTON:
            switch (state)
            {
                case GLUT_DOWN: // Immediately remove planet(s) at pressed position
                {
                    Physics::Vector2d clicked_model_pos = Physics::Vector2d((x - window_width/2) * model_scale,
                                                                            (y - window_height/2) * model_scale);
                    pair<bool, unsigned int> ret = pSimpleSpace->find_planet_by_click(clicked_model_pos);
                    if (ret.first) {
                        pSimpleSpace->remove_planet(ret.second);
                    }
                    break;
                }
                case GLUT_UP:
                {
                    if ((x - mouse_right_key_down_x != 0 ) && (y - mouse_right_key_down_y) != 0) {
                        Physics::Vector2d sel_start_model_pos = Physics::Vector2d((mouse_right_key_down_x - window_width/2) * model_scale,
                                                                                  (mouse_right_key_down_y - window_height/2) * model_scale);
                        Physics::Vector2d sel_end_model_pos   = Physics::Vector2d((x - window_width/2) * model_scale,
                                                                                  (y - window_height/2) * model_scale);
                        std::vector<unsigned int> id_list = pSimpleSpace->find_planets_by_selection(sel_start_model_pos, sel_end_model_pos);
                        for (std::vector<unsigned int>::iterator it = id_list.begin(), it_end = id_list.end(); it != it_end; ++it) {
                            pSimpleSpace->remove_planet(*it);
                        }
                    }
                }
                break;
            }
            break;
    }
}

// Mouse motion while some keys are being pressed
void handleMouseActiveMotion(int x, int y) {
    mouse_active_motion_x = x;
    mouse_active_motion_y = y;

    mouse.x = x;
    mouse.y = y;
    controls.update_controls(mouse);

    if (mouse_left_key_is_down) {
        if (mass_modifier_key_down) {
            next_planet.mass_kg = sqrt(pow((x - mouse_left_key_down_x), 2) + \
                                       pow((y - mouse_left_key_down_y), 2)) * 1e30;
        } else if (rad_modifier_key_down) {
            next_planet.rad_m = sqrt(pow((x - mouse_left_key_down_x) * model_scale, 2) + \
                                     pow((y - mouse_left_key_down_y) * model_scale, 2));
        } else {
            next_planet.vel.x = (x - mouse_left_key_down_x) * model_scale;
            next_planet.vel.y = (y - mouse_left_key_down_y) * model_scale;
        }
    }
}

// Mouse motion without keys being pressed
void handleMousePassiveMotion(int x, int y) {
    mouse.x = x;
    mouse.y = y;
    controls.update_controls(mouse);
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

// default changed to make glutInit() work
//int main(int argc, const char * argv[])
int main(int argc, char * argv[])
{
    cout << "main(): Stared" << endl;

    // Seed for random values
    srand(static_cast<unsigned int>(time(NULL)));

    // SimpleSpace testing begin
    double dist = 4e7;
    pSimpleSpace->add_planet(Planet(Vector2d(0, 0), Vector2d(0, 0), 1e30, 3e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d( dist/4,   0), Vector2d(0,   -2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(-dist/4,   0), Vector2d(0,    2e6), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0,  dist/1.5), Vector2d(-1.5e6, 0), 1e15, 1e6, getRandomColor()));
    pSimpleSpace->add_planet(Planet(Vector2d(0, -dist/1.5), Vector2d( 1.5e6, 0), 1e15, 1e6, getRandomColor()));

    controls.add_button(20, 200, 80, 30, "button 1", button_func);
    controls.add_button(20, 250, 80, 30, "button 2", button_func);

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(1024, 600);

    glutCreateWindow("Simple Space");
    initRendering();

    glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);

    glutKeyboardFunc(handleNormalKeysDown);
    glutKeyboardUpFunc(handleNormalKeysUp);
    glutSpecialFunc(handleSpecialKeysDown);

    glutMouseFunc(handleMouseKeypress);
    glutMotionFunc(handleMouseActiveMotion);
    glutPassiveMotionFunc(handleMousePassiveMotion);

    glutIgnoreKeyRepeat(1);
    //glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF); // Disables globally - for other apps, so don't use

    glutTimerFunc(1000/FRAMERATE, update, 0); //Add a timer

    glutMainLoop();

    // Delete global SimpleSpace
    // TODO: find where deinit should be, as we don't get here
    delete pSimpleSpace;
    pSimpleSpace = NULL;

    cout << "main(): Finished" << endl;
    return 0;
}
