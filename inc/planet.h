//
//  Planet.h
//  simple-space
//
//  Created by Vladimir Frolov on 08.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __simple_space__planet__
#define __simple_space__planet__

#include <iostream>
#include <string>
using std::string;

#include "physics.h"
using physics::Position;
using physics::Velocity;
using physics::Force;

// Earth
#define EARTH_MASS_KG 5.9722e24
#define EARTH_RAD_M   6.371e6

// Moon
#define MOON_MASS_KG 7.3477e22
#define MOON_RAD_M   1.737e6

// Sun
#define SUN_MASS_KG 1.9891e30
#define SUN_RAD_M   6.955e8


enum PlanetName {
    EARTH,
    MOON,
    SUN
};


class Planet : public physics::Body {
public:
    // Constructor
    Planet(string Name = "Earth",
           double MassKg = EARTH_MASS_KG,
           double RadM = EARTH_RAD_M,
           Position Pos = Position(),
           Velocity Vel = Velocity()) : Body(Name, Pos, Vel), massKg(MassKg), radM(RadM), newPos(Pos) {}

    // Mass and Radius
    double massKg;
    double radM;

    // Newly calculated position
    Position newPos;
};

#endif /* defined(__simple_space__planet__) */
