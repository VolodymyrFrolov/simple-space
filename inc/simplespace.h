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

#include "physics.h"
#include "planet.h"
using physics::phys_vector;

#define ENABLE_GRAVITY 1    // Gravity: 1-on; 0-off
#define E_COEF 1            // Coefficient of restitution [0-1] 1-absolute elastic

#define ENABLE_BORDERS 1    // Borders: 1-on; 0-off
#define LEFT_BORDER   -5e7
#define RIGHT_BORDER   5e7
#define TOP_BORDER     5e7
#define BOTTOM_BORDER -5e7

class SimpleSpace
{
public:
    SimpleSpace();

    void addPlanet(const Planet& newPlanet);
    void MoveOneStep();
    vector<Planet> planets;
    // TODO: make private members
private:
    int _timeStepMs;
};

#endif /* defined(__simple_space__simplespace__) */
