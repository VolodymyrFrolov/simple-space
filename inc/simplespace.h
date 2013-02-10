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
#include <list>
using std::list;
using std::pair;
using std::cout;  // temp
using std::endl; // temp
#include <stdlib.h> // For rand()

#include "physics.h"
#include "planet.h"
using physics::Position;
using physics::Velocity;
using physics::Force;
using physics::Acceleration;


class SimpleSpace
{
public:
    SimpleSpace();

    void ShowLogs(); //temp
    void addPlanet();
    void addPlanet(const Planet& newPlanet);
    void MoveOneStep();
private:
    list<Planet> PlanetList;
    int _timeStepMs;
    
};

#endif /* defined(__simple_space__simplespace__) */
