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
#include <pthread.h>

#include "physics.h"
#include "planet.h"
using physics::phys_vector;

#define ENABLE_GRAVITY 1    // Gravity: 1-on; 0-off
#define COEF_RES       0.5  // Coefficient of restitution [0..1] = [absolutely inelastic .. absolute elastic]

#define ENABLE_BORDERS 1    // Borders: 1-on; 0-off
#define LEFT_BORDER   -8e7
#define RIGHT_BORDER   8e7
#define TOP_BORDER     5e7
#define BOTTOM_BORDER -5e7

class SimpleSpace
{
public:
    SimpleSpace(int timestep_ms = 10);
    ~SimpleSpace();
    void add_planet(const Planet& new_planet);
    void add_planet_by_Pos_and_Vel(const phys_vector& pos, const phys_vector& vel);
    void remove_all_objects();
    void move_one_step();
    vector<Planet> planets;
    // TODO: make private members
    int  get_model_timestep_ms() const;
    void set_model_timestep_ms(int timestep_ms);

private:
    int planet_id;
    pthread_mutex_t step_mutex;
    void pull_apart_planets(Planet& p1, Planet& p2);
    void resolve_border_collision(Planet& p);
    int _timestep_ms;
};

#endif /* defined(__simple_space__simplespace__) */
