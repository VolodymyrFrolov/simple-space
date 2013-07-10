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
#include <stdlib.h> // For rand()
using std::cout;   // temp
using std::endl;   // temp
using std::vector;
#include <mutex>

#include "physics.h"
#include "planet.h"
using physics::phys_vector;

#define GRAVITY_ENABLED  0    // Gravity: 1-on; 0-off
#define COEF_RES         0.5  // Coefficient of restitution [0..1] = [absolutely inelastic .. absolute elastic]

#define BORDERS_ENABLED  1    // Borders: 1-on; 0-off
#define BORDER_FRICTION  0.5  // Friction of borders: 0..1
#define LEFT_BORDER     -8e7
#define RIGHT_BORDER     8e7
#define TOP_BORDER       5e7
#define BOTTOM_BORDER   -5e7

#define GLOBAL_GRAVITY_ACC_X 1e5
#define GLOBAL_GRAVITY_ACC_Y 1e5

class SimpleSpace
{
    int planet_id;
    std::mutex step_mutex;
    void move_apart_bodies(Planet& p1, Planet& p2);
    void move_apart_bodies_v2(Planet& p1, Planet& p2);
    void resolve_border_collision(Planet& p);
    double _time_step_ms;

public:
    SimpleSpace(int timestep_ms = 10);
    ~SimpleSpace();
    void add_planet(const Planet& new_planet);
    void add_planet_by_Pos_and_Vel(const phys_vector& pos, const phys_vector& vel, const double Ang_Vel, const ColorF_RGB col);
    void remove_all_objects();
    void move_one_step();
    vector<Planet> planets;

    // TODO: move to private
    int  get_model_time_step_ms() const;
    void set_model_time_step_ms(int time_step_ms);
};

#endif /* defined(__simple_space__simplespace__) */
