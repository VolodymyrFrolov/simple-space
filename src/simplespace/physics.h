//
//  physics.h
//  simple-space
//
//  Created by Vladimir Frolov on 08.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __simple_space__physics__
#define __simple_space__physics__

#include <iostream>
#include <math.h>
using std::pair;
using std::string;

#define CONST_G 6.67385e-11
#define DevByZero -1

namespace Physics {

    struct Vector2d {
        Vector2d(double new_x = 0,
                 double new_y = 0) : x(new_x), y(new_y) {}

        bool operator==(const Vector2d &rhs) const {
            return ((x == rhs.x) && (y == rhs.y));
        }

        bool operator!=(const Vector2d &rhs) const {
            return ((x != rhs.x) || (y != rhs.y));
        }

        double x;
        double y;
    };

    double DegToRad(const double& Deg);
    double RadToDeg(const double& Rad);
    double Hypotenuse(const double& a, const double& b);
    
    void RotateVector(Vector2d& vec, const double& angle);
    void TranslateVector(Vector2d& vec, const double& dx, const double& dy);

    // Distance by two points: x0,y0 and x1,y1
    // Distance by two points: pos1, pos2
    double DistFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    double DistFromPos(const Vector2d& pos0, const Vector2d& pos1);

    // Angle [0 - 2*Pi] (Rad) of line by two points: (x0,y0), (x1,y1)
    // Angle [0 - 2*Pi] (Rad) of line by two points: pos1, pos2
    double AngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    double AngleFromPos(const Vector2d& pos0, const Vector2d& pos1);

    // Input: coordinates of two points (x0,y0; x1,y1)
    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    pair<double, double> DistAngleFromPos(const Vector2d& pos0, const Vector2d& pos1);

    // Gravity acceleration, m/s^2
    double GravAcc(const double& massKg, const double& distM);
    // Gravity force, N
    double GravForce(const double& mass1Kg, const double& mass2Kg, const double& distM);

    // Movement with constant acceleration
    void MoveWithConstAcc(Vector2d& pos,
                          Vector2d& vel,
                          const Vector2d& acc,
                          const double& time);
} // namespace physics

#endif
