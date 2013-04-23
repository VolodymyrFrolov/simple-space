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

int mouse_x_pressed = 0;
int mouse_y_pressed = 0;
int mouse_x_current = 0;
int mouse_y_current = 0;
bool mouse_pressed_left = false;
bool mouse_pressed_right = false;

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

void DrawFilledCircle(GLdouble rad, int segments, GLdouble x_center, GLdouble y_center);

//Initializes rendering
void initRendering()
{
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
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    glTranslated(window_width/2.0, window_height/2.0, 0.0);
    
    //glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f); //Rotate the camera
    //glPushMatrix(); //Save the transformations performed thus far
    //glPopMatrix(); //Undo the move to the center
    
    for (vector<Planet>::const_iterator it = pSimpleSpace->planets.begin(),
        it_end = pSimpleSpace->planets.end(); it != it_end; ++it) {
        DrawFilledCircle(it->radM/double(model_scale), 20, it->pos.x/double(model_scale), it->pos.y/double(model_scale));
    }

    if (mouse_pressed_left) {
        glBegin(GL_LINES);
        glVertex2d(mouse_x_pressed - window_width/2.0, mouse_y_pressed - window_height/2.0);
        glVertex2d(mouse_x_current - window_width/2.0, mouse_y_current - window_height/2.0);
        glEnd();
    }


    #if (ENABLE_BORDERS > 0)
    glBegin(GL_LINE_LOOP);
    glVertex2d(RIGHT_BORDER/double(model_scale), TOP_BORDER/double(model_scale));
    glVertex2d(LEFT_BORDER/double(model_scale), TOP_BORDER/double(model_scale));
    glVertex2d(LEFT_BORDER/double(model_scale), BOTTOM_BORDER/double(model_scale));
    glVertex2d(RIGHT_BORDER/double(model_scale), BOTTOM_BORDER/double(model_scale));
    glEnd();
    #endif

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
	switch (key) {
        case 'c':
            pSimpleSpace->remove_all_objects();
			break;
		case 27:  // Escape key
        case 'q':
            delete pSimpleSpace;
            pSimpleSpace = NULL;
            cout << "handleKeypress(): exit(0)" << endl;
			exit(0);
	}
}

void handleSpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_RIGHT:
            if (!(model_speed * pSimpleSpace->get_model_timestep_ms() > 100000)) {
                model_speed *= 10;
                cout << "model speed: " << model_speed << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
            }
            break;
        case GLUT_KEY_LEFT:
            if (model_speed > 1) {
                model_speed /= 10;
                cout << "model speed: " << model_speed << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
            }
            break;
        case GLUT_KEY_UP:
            model_scale /= 2;
            cout << "model scale: " << model_scale << endl;
            break;
        case GLUT_KEY_DOWN:
            model_scale *= 2;
            cout << "model scale: " << model_scale << endl;
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

    // Save mouse press position
    if ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON)) {
        mouse_x_pressed = x;
        mouse_y_pressed = y;
    }

    // Add planet when left mouse button release
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
                                                phys_vector(mouse_moved_x * model_scale, mouse_moved_y * model_scale));
        cout << " objects: "<< pSimpleSpace->planets.size() << " (" << FRAMERATE * model_speed * pSimpleSpace->planets.size() << " calcs per second)" << endl;
    }
}

void handleMouseMotion(int x, int y) {
    mouse_x_current = x;
    mouse_y_current = y;
}

//Draw a 2D painted cicle using GL_TRIANGLE_FAN
void DrawFilledCircle(GLdouble rad, int segments, GLdouble x_center, GLdouble y_center)
{
    // Input data check
    segments = M_PI_2 * rad;
    if (rad < 1)
        rad = 1;
    if (segments < 10)
        segments = 10;
    
    GLdouble x = 0;
    GLdouble y = 0;
    
    // Drawing the circle
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d( x_center, y_center );
    for (int i = 0; i <= segments; ++i)
    {
        x = rad * cos( (i * 2 * M_PI) / double(segments) );
        y = rad * sin( (i * 2 * M_PI) / double(segments) );
        glVertex2d( x+x_center, y+y_center );
    }
    glEnd();
}

// default changed to make glutInit() work
//int main(int argc, const char * argv[])
int main(int argc, char * argv[])
{
    cout << "main(): Stared" << endl;

    // Seed for random values
    //srand ( (unsigned int)(time(NULL)) );

    // SimpleSpace testing begin
    double dist = 4e7;
    pSimpleSpace->add_planet(Planet("Planet-1", 1e30, 3e6, phys_vector(0, 0), phys_vector(0, 0)));
    pSimpleSpace->add_planet(Planet("Planet-2", 1e15, 1e6, phys_vector(dist/4,   0), phys_vector(0,  -2e6)));
    pSimpleSpace->add_planet(Planet("Planet-2", 1e15, 1e6, phys_vector(-dist/4,  0), phys_vector(0,   2e6)));
    pSimpleSpace->add_planet(Planet("Planet-4", 1e15, 1e6, phys_vector(0, dist/1.5), phys_vector(-1.5e6, 0)));
    pSimpleSpace->add_planet(Planet("Planet-5", 1e15, 1e6, phys_vector(0, -dist/1.5), phys_vector(1.5e6, 0)));

    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(1024, 600);

	glutCreateWindow("My test");
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
