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
using Physics::Vector2d;

// Earth
#define EARTH_MASS_KG 5.9722e24
#define EARTH_RAD_M   6.371e6

// Moon
#define MOON_MASS_KG 7.3477e22
#define MOON_RAD_M   1.737e6

// Sun
#define SUN_MASS_KG 1.9891e30
#define SUN_RAD_M   6.955e8

struct Color_RGB {
    float R;
    float G;
    float B;
};

struct Color_RGBA {
    float R;
    float G;
    float B;
    float A;
};

struct Planet {
    Planet(Vector2d Pos = Vector2d(),
           Vector2d Vel = Vector2d(),
           double Mass_Kg = 0,
           double Rad_M = 0,
           Color_RGB Color = {1.0f, 1.0f, 1.0f})
    : id(0),
      pos(Pos),
      vel(Vel),
      mass_kg(Mass_Kg),
      rad_m(Rad_M),
      prev_pos(Pos),
      color(Color) {}

    unsigned int id;
    Vector2d pos;
    Vector2d prev_pos;
    Vector2d vel;
    double mass_kg;
    double rad_m;
    Color_RGB color;

    void reset_parameters() {
        pos = Vector2d();
        prev_pos = Vector2d();
        vel = Vector2d();
        mass_kg = 0;
        rad_m = 0;
        color = {1.0f, 1.0f, 1.0f};
    }
};

#endif /* defined(__simple_space__planet__) */
