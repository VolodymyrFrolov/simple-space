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
#include <mutex>

#include "physics.h"
#include "planet.h"
using Physics::Vector2d;

#define GRAVITY_ENABLED  1    // Gravity: 1-on; 0-off
#define COEF_RES         0.7  // Coefficient of restitution [0..1] = [absolutely inelastic .. absolute elastic]

#define BORDERS_ENABLED  1    // Borders: 1-on; 0-off
#define BORDER_FRICTION  0.7  // Friction of borders: 0..1
#define LEFT_BORDER     -8e7
#define RIGHT_BORDER     8e7
#define TOP_BORDER       5e7
#define BOTTOM_BORDER   -5e7

#define GLOBAL_TOP_MASS    1e30 // put 1e32 for both to reprocuce crash whenplnets get to the corner
#define GLOBAL_RIGHT_MASS  1e29    // temp, for physics check

class SimpleSpace
{
    void move_apart_bodies(Planet& p1, Planet& p2);
    void move_apart_bodies_v2(Planet& p1, Planet& p2);
    void resolve_border_collision(Planet& p);
    //std::vector<unsigned int> get_id_list(); this method is not uesd

    std::mutex movement_step_mutex;
    double time_step_ms;

public:
    SimpleSpace(int timestep_ms = 10);
    ~SimpleSpace();
    void add_planet(const Planet& pl);
    void remove_planet(const unsigned int& id);
    void remove_all_objects();
    void move_one_step();
    int  get_model_time_step_ms() const;

    std::vector<Planet> planets;
    const unsigned int planets_number_max; // std::numeric_limits<unsigned int>::max()
};

#endif /* defined(__simple_space__simplespace__) */
