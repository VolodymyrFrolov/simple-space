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


    // Position, Velocity and Acceleration structures
    struct Position
    {
        Position(double coord_x = 0,
                 double coord_y = 0) : x(coord_x), y(coord_y) {}
        double x;
        double y;
    };

    struct Velocity
    {
        Velocity(double vel_x = 0,
                 double vel_y = 0) : x(vel_x), y(vel_y) {}
        double x;
        double y;
    };

    struct Force
    {
        Force(double force_x = 0,
              double force_y = 0) : x(force_x), y(force_y) {}
        double x;
        double y;
    };

    struct Acceleration
    {
        Acceleration(double accel_x = 0,
                     double accel_y = 0) : x(accel_x), y(accel_y) {}
        double x;
        double y;
    };


    // Body class
    class Body
    {
    public:
        // Constructor
        Body(string Name = "unnamed",
             Position Pos = Position(),
             Velocity Vel = Velocity()) : name(Name), pos(Pos), vel(Vel) {};

        string name;
        Position pos;
        Velocity vel;
    };

    // Angle conversions: Deg->Rad & Rad->Deg
    double DegToRad(const double& Deg);
    double RadToDeg(const double& Rad);

    // Hypotenoose: c = sqrt( a^2 + b^2 )
    double Hypotenuse(const double& a, const double& b);

    // Distance by two points: x0,y0 and x1,y1
    double DistFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    // Distance by two points: pos1, pos2
    double DistFromPos(const Position& pos0, const Position& pos1);

    // Angle (in Radians) of line by two points: (x0,y0), (x1,y1)
    double AngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    // Angle (in Radians) of line by two points: pos1, pos2
    double AngleFromPos(const Position& pos0, const Position& pos1);

    // Input: coordinates of two points (x0,y0; x1,y1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1);
    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const Position& pos0, const Position& pos1);

    // Gravity acceleration, m/s^2
    double GravAcc(const double& massKg, const double& distM);
    // Gravity force, N
    double GravForce(const double& mass1Kg, const double& mass2Kg, const double& distM);

    // Movement with constant acceleration
    void MoveWithConstAcc(Position& pos, Velocity& vel, const Acceleration& acc, const double& time);

} // namespace physics

#endif