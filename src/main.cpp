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

// Creating global SimpleSpace
SimpleSpace * pSimpleSpace = new SimpleSpace;

const int FRAMERATE = 60;
int animation_accel = 3000;

void initRendering();
void update(int value);
void drawScene();
void handleResize(int w, int h);
void handleKeypress(unsigned char key, int x, int y);
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
    for (int i = 0; i < animation_accel; ++i) {
        pSimpleSpace->move_one_step();
    }
    
    glutPostRedisplay(); //Tell GLUT that the display has changed
    
    glutTimerFunc( 1000/FRAMERATE, update, 0);
}

//Draws the 3D scene
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    int window_width = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);
    glTranslatef( window_width/2.0f, window_height/2.0f, 0.0f);
    
    //glRotatef(-_cameraAngle, 0.0f, 1.0f, 0.0f); //Rotate the camera
    //glPushMatrix(); //Save the transformations performed thus far
    //glPopMatrix(); //Undo the move to the center
    
    //DrawFilledCircle(5);
    
    double coef = 200000;
    
    for (vector<Planet>::const_iterator it = pSimpleSpace->planets.begin(),
         it_end = pSimpleSpace->planets.end(); it != it_end; ++it)
    {
        DrawFilledCircle(it->radM/coef, 20, it->pos.x/coef, it->pos.y/coef);
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
void handleKeypress(unsigned char key, int x, int y)
{
	switch (key) {
		case 27: //Escape key
			exit(0);
	}
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
    cout << "Simple-Space: main Stared" << endl;

    // Seed for random values
    //srand ( (unsigned int)(time(NULL)) );


    // SimpleSpace testing begin
    double dist = 4e7;
    pSimpleSpace->add_planet(Planet("P1", EARTH_MASS_KG   , EARTH_RAD_M, phys_vector(0, 0), phys_vector(0,0)));
    pSimpleSpace->add_planet(Planet("P2", EARTH_MASS_KG/5 , EARTH_RAD_M/2, phys_vector(dist,dist), phys_vector(-1000,-500)));
    pSimpleSpace->add_planet(Planet("P3", EARTH_MASS_KG*2 , EARTH_RAD_M*1.5, phys_vector(-dist,-dist), phys_vector(200,1000)));

    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	
	glutCreateWindow("My test");
	initRendering();
    
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutReshapeFunc(handleResize);
    
	glutTimerFunc(1000/FRAMERATE, update, 0); //Add a timer
	
	glutMainLoop();

    // Delete global SimpleSpace
    // TODO: find where deinit should be, as we don't get here
    delete pSimpleSpace;
    pSimpleSpace = NULL;
    cout << "Simple-Space: main Finished" << endl;
    return 0;
}
