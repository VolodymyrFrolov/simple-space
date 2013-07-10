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
using std::vector;

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

#include "simplespace.h"
#include "planet.h"
#include "physics.h"

const int FRAMERATE = 60;
int model_speed = 1;
int model_scale = 200000;
int view_offset_x = 0;
int view_offset_y = 0;

int mouse_x_pressed = 0;
int mouse_y_pressed = 0;
int mouse_x_current = 0;
int mouse_y_current = 0;
bool mouse_pressed_left = false;
bool mouse_pressed_right = false;

bool shift_pressed = false;
bool ctrl_pressed = false;
bool alt_pressed = false;

const double default_planet_mass = 1e29;
const double default_planet_rad = 2e6;
double mass_for_new_planet = default_planet_mass;
double rad_for_new_planet = default_planet_rad;
Color_RGB next_color = {0.0f, 0.0f, 0.0f};

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
void handleMouse(int button, int state, int x, int y);
void handleMouseMotion(int x, int y);

void render_bitmap_string_2d(const char * cstr, float x, float y, void * font, Color_RGBA color);
void draw_planet(GLdouble radius, GLdouble centre_x, GLdouble centre_y, Color_RGBA color, GLdouble angle);

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
    glTranslated(window_width/2.0 + view_offset_x, window_height/2.0 + view_offset_y, 0.0);
    
    //glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f); //Rotate the camera
    //glPushMatrix(); //Save the transformations performed thus far
    //glPopMatrix(); //Undo the move to the center
    
    for (vector<Planet>::const_iterator it = pSimpleSpace->planets.begin(),
        it_end = pSimpleSpace->planets.end(); it != it_end; ++it) {
        draw_planet(it->radM/model_scale, it->pos.x/model_scale, it->pos.y/model_scale, {it->color.R, it->color.G, it->color.B, 1.0f}, it->angle);
    }

    if (mouse_pressed_left) {        
        draw_planet(rad_for_new_planet/model_scale, mouse_x_pressed - window_width/2.0, mouse_y_pressed - window_height/2.0, \
                    {next_color.R, next_color.G, next_color.B, 1.0f}, 0);
        render_bitmap_string_2d("Rad: xxx Mass: xxx",
                                mouse_x_pressed - window_width/2.0 - 50,
                                mouse_y_pressed - window_height/2.0 + 22,
                                GLUT_BITMAP_HELVETICA_12,
                                {1.0f, 1.0f, 1.0f, 0.5f});
        glBegin(GL_LINES);
        glColor4f(next_color.R, next_color.G, next_color.B, 1.0f);
        glVertex2d(mouse_x_pressed - window_width/2.0, mouse_y_pressed - window_height/2.0);
        glVertex2d(mouse_x_current - window_width/2.0, mouse_y_current - window_height/2.0);
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
void handleNormalKeys(unsigned char key, int x, int y) {
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
	}
}

void handleSpecialKeys(int key, int x, int y) {
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

void handleMouse(int button, int state, int x, int y) {

    mouse_x_current = x;
    mouse_y_current = y;

    if (button == GLUT_LEFT_BUTTON) {
        mouse_pressed_left = (state == GLUT_DOWN);
    } else if (button == GLUT_RIGHT_BUTTON) {
        mouse_pressed_right = (state == GLUT_DOWN);
    }
    
    //shift_pressed = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
    //ctrl_pressed = glutGetModifiers() & GLUT_ACTIVE_CTRL;
    //alt_pressed = glutGetModifiers() & GLUT_ACTIVE_ALT;

    // Save mouse pressed down position
    if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON)) {
        mouse_x_pressed = x;
        mouse_y_pressed = y;
    }
    
    if (mouse_pressed_left)
        next_color = getRandomColor();

    // Add planet when left mouse button released
    if ((state == GLUT_UP) && (button == GLUT_LEFT_BUTTON)) {
        // Calculate model coordinates
        int window_width = glutGet(GLUT_WINDOW_WIDTH);
        int window_height = glutGet(GLUT_WINDOW_HEIGHT);
        int model_x = (mouse_x_pressed - window_width/2) * model_scale;
        int model_y = (mouse_y_pressed - window_height/2) * model_scale;

        // Calculate speed depending on mouse grag distance
        int mouse_moved_x = x - mouse_x_pressed;
        int mouse_moved_y = y - mouse_y_pressed;
        cout << "Drag X=" << mouse_moved_x << " Y=" << mouse_moved_y << endl;

        pSimpleSpace->add_planet_by_Pos_and_Vel(phys_vector(model_x, model_y),
                                                phys_vector(mouse_moved_x * model_scale, mouse_moved_y * model_scale),
                                                0,
                                                next_color);
        cout << " objects: "<< pSimpleSpace->planets.size() << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
    }
}

void handleMouseMotion(int x, int y) {
    if (mouse_pressed_left) {
        mouse_x_current = x;
        mouse_y_current = y;
    }
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
void draw_planet(GLdouble rad, GLdouble centre_x, GLdouble centre_y, Color_RGBA color, GLdouble angle)
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

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2d(centre_x, centre_y);
    glVertex2d(rad * cos(angle) + centre_x, rad * sin(angle) + centre_y);
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
    pSimpleSpace->add_planet(Planet("Planet-1", 1e30, 3e6, phys_vector(0, 0), phys_vector(0, 0), 0, getRandomColor()));
    pSimpleSpace->add_planet(Planet("Planet-2", 1e15, 1e6, phys_vector(dist/4,   0), phys_vector(0,  -2e6), 0, getRandomColor()));
    pSimpleSpace->add_planet(Planet("Planet-2", 1e15, 1e6, phys_vector(-dist/4,  0), phys_vector(0,   2e6), 0, getRandomColor()));
    pSimpleSpace->add_planet(Planet("Planet-4", 1e15, 1e6, phys_vector(0, dist/1.5), phys_vector(-1.5e6, 0), 0, getRandomColor()));
    pSimpleSpace->add_planet(Planet("Planet-5", 1e15, 1e6, phys_vector(0, -dist/1.5), phys_vector(1.5e6, 0), 0, getRandomColor()));

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowSize(1024, 600);

	glutCreateWindow("Simple Space");
	initRendering();

	glutDisplayFunc(drawScene);
    glutReshapeFunc(handleResize);

	glutKeyboardFunc(handleNormalKeys);
    glutSpecialFunc(handleSpecialKeys);
    glutMouseFunc(handleMouse);
    glutMotionFunc(handleMouseMotion);

	glutTimerFunc(1000/FRAMERATE, update, 0); //Add a timer

	glutMainLoop();

    // Delete global SimpleSpace
    // TODO: find where deinit should be, as we don't get here
    delete pSimpleSpace;
    pSimpleSpace = NULL;

    cout << "main(): Finished" << endl;
    return 0;
}
