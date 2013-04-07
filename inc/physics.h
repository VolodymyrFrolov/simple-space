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

namespace physics {
   
    // Physics vector structure
    struct phys_vector {
        phys_vector(double new_x = 0,
                    double new_y = 0) : x(new_x), y(new_y) {}
        bool operator==(const phys_vector &rhs) const {
            return ((x == rhs.x) && (y == rhs.y));
        }
        double x;
        double y;
    };

    // Body class
    class Body
    {
    public:
        // Constructor
        Body(string Name,
             phys_vector Pos,
             phys_vector Vel) : name(Name), pos(Pos), vel(Vel) {};

        string name;
        phys_vector pos;
        phys_vector vel;
    };

    double DegToRad(const double& Deg);
    double RadToDeg(const double& Rad);
    double Hypotenuse(const double& a, const double& b);
    
    void RotateVector(phys_vector& vec, const double& angle);
    void TranslateVector(phys_vector& vec, const double& dx, const double& dy);

    // Distance by two points: x0,y0 and x1,y1
    // Distance by two points: pos1, pos2
    double DistFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    double DistFromPos(const phys_vector& pos0, const phys_vector& pos1);

    // Angle [0 - 2*Pi] (Rad) of line by two points: (x0,y0), (x1,y1)
    // Angle [0 - 2*Pi] (Rad) of line by two points: pos1, pos2
    double AngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    double AngleFromPos(const phys_vector& pos0, const phys_vector& pos1);

    // Input: coordinates of two points (x0,y0; x1,y1)
    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    pair<double, double> DistAngleFromPos(const phys_vector& pos0, const phys_vector& pos1);

    // Gravity acceleration, m/s^2
    double GravAcc(const double& massKg, const double& distM);
    // Gravity force, N
    double GravForce(const double& mass1Kg, const double& mass2Kg, const double& distM);

    // Movement with constant acceleration
    void MoveWithConstAcc(phys_vector& pos, phys_vector& vel, const phys_vector& acc, const double& time);

} // namespace physics

#endif
