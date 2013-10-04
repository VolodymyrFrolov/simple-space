//
//  spacemanager.h
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __simple_space__simplespace__
#define __simple_space__simplespace__

#include <iostream>
#include <vector>
//#include <stdlib.h> // For rand()
using std::cout;   // temp
using std::endl;   // temp
#include <mutex>

#ifdef __APPLE__
    #include <OpenGL/OpenGL.h>
    #include <GLUT/glut.h>
#elif __linux__
  //#include <GL/glut.h>
    #include <GL/freeglut.h>
#else
    // Unsupproted platform
#endif

#include "mouse_and_keyboard.h"
#include "planet.h"
#include "physics.h"
using Physics::Vector2d;

#define GRAVITY_ENABLED  1    // Gravity: 1-on; 0-off
#define COEF_RES         0.7  // Coefficient of restitution [0..1] = [absolutely inelastic .. absolute elastic]

#define BORDERS_ENABLED  1    // Borders: 1-on; 0-off
#define BORDER_FRICTION  0.7  // Friction of borders: 0..1
#define LEFT_BORDER     -8e7
#define RIGHT_BORDER     8e7
#define TOP_BORDER       5e7
#define BOTTOM_BORDER   -5e7

#define GLOBAL_TOP_MASS    0 //1e30 // put 1e32 for both to reprocuce crash whenplnets get to the corner
#define GLOBAL_RIGHT_MASS  0 //1e29    // temp, for physics check

class SimpleSpace
{
    void move_apart_bodies(Planet& p1, Planet& p2);
    void resolve_body_collision(Planet& pla, Planet& plb);
    void check_and_resolve_border_collision(Planet& pl);

    std::mutex movement_step_mutex;
    double time_step_ms;

    void draw_planet(const float& rad, const float& x, const float& y) const;
public:
    SimpleSpace(int timestep_ms = 10);
    ~SimpleSpace();
    void add_planet(const Planet& pl);
    void remove_planet(const unsigned int& id);
    void remove_all_objects();
    void move_one_step();

    unsigned long get_planets_count() const;
    int get_model_time_step_ms() const;
    std::pair<bool, unsigned int> find_planet_by_click(const Vector2d& click_pos);
    std::vector<unsigned int> find_planets_by_selection(const Vector2d& sel_start_pos,
                                                        const Vector2d& sel_end_pos);

    std::vector<Planet> planets;
    const unsigned int planets_number_max; // std::numeric_limits<unsigned int>::max()

    void handle_mouse_move(const Mouse& mouse);
    void handle_mouse_key_event(const Mouse& mouse, MOUSE_KEY key, KEY_ACTION action);
    void handle_keyboard_key_event(char key, KEY_ACTION action);
    void draw_scene(const float& scale) const;
};

#endif /* defined(__simple_space__simplespace__) */
