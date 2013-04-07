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
using physics::phys_vector;

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
    Planet(string Name,
           double MassKg,
           double RadM,
           phys_vector Pos,
           phys_vector Vel) : Body(Name, Pos, Vel), massKg(MassKg), radM(RadM), temp_pos(Pos) {}

    // Mass and Radius
    double massKg;
    double radM;

    // temp position for gravity calculations
    phys_vector temp_pos;
    void apply_temp_pos();

    // TODO: make private members
};

#endif /* defined(__simple_space__planet__) */
