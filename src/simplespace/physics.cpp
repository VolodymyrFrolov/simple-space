//
//  physics.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 08.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "physics.h"

namespace Physics {

    inline double DegToRad(const double& Deg) { return (Deg * double(M_PI)) / double(180); }
    inline double RadToDeg(const double& Rad) { return (Rad * 180) / double(M_PI); }
    inline double Hypotenuse(const double& a, const double& b) { return sqrt((a*a) + (b*b)); }

    void RotateVector(Vector2d& vec, const double& angle)
    {
        Vector2d temp(vec.x * cos(angle) - vec.y * sin(angle),    // x` = x * cos(fi) - y * sin(fi)
                         vec.x * sin(angle) + vec.y * cos(angle));   // x` = x * sin(fi) + y * cos(fi)
        vec = temp;
    }
    
    void TranslateVector(Vector2d& vec, const double& dx, const double& dy)
    {
        vec.x += dx;
        vec.y += dy;
    }

    // Distance by two points: x0,y0 and x1,y1
    double DistFromPos(const double& x0, const double& y0, const double& x1, const double& y1)
    {
        double tmpX = x1 - x0;
        double tmpY = y1 - y0;
        return Hypotenuse( tmpX, tmpY );
    }

    // Distance by two points: pos1, pos2
    double DistFromPos(const Vector2d& pos0, const Vector2d& pos1)
    {
        return DistFromPos( pos0.x, pos0.y, pos1.x, pos1.y );
    }


    // Angle [0 - 2*Pi] (Rad) of line by two points: (x0,y0), (x1,y1)
    double AngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1)
    {
        double relX = x1 - x0;
        double relY = y1 - y0;

        // Case of same position
        if ( (relX == 0) && (relY == 0) )
            return 0;

        // Angle calculation usung atan2(): algo #1
        double ret = atan2(relY, relX); // atan2(Y,X) is correct
        if (ret < 0)
            ret += 2*M_PI;
        return ret;

        // Angle calculation usung atan(): algo #2
        /*
        if (relX < 0) {
            return (double(M_PI) + atan(relY/relX));
        } else {
            if (relY < 0)
                return (2 * double(M_PI) + atan(relY/relX));
            else
                return atan(relY/relX);
        }
        */
    }

    // Angle [0 - 2*Pi] (Rad) of line by two points: pos1, pos2
    double AngleFromPos(const Vector2d& pos0, const Vector2d& pos1) { return AngleFromPos( pos0.x, pos0.y, pos1.x, pos1.y ); }

    // Input: coordinates of two points (x0,y0; x1,y1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const double& x0, const double& y0, const double& x1, const double& y1)
    {
        // Pair of return values:
        // .first = Distance; .second = Angle;
        pair<double,double> ret;

        // Distance
        ret.first = DistFromPos( x0, y0, x1, y1 );

        // Angle
        ret.second = AngleFromPos( x0, y0, x1, y1 );

        return ret;
    }

    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    pair<double, double> DistAngleFromPos(const Vector2d& pos0, const Vector2d& pos1) { return DistAngleFromPos( pos0.x, pos0.y, pos1.x, pos1.y ); }

    // Gravity acceleration, m/s^2
    double GravAcc(const double& massKg, const double& distM)
    {
        if (distM == 0) {
            std::cout << "ERROR: devision by zero attempted in GravAcc()! Throwing exception!" << std::endl;
            throw (DevByZero);
        }
        return (double(CONST_G) * massKg) / (distM * distM);
    }

    // Gravity force, N [Not currently used]
    double GravForce(const double& mass1Kg, const double& mass2Kg, const double& distM)
    {
        if (distM == 0) {
            std::cout << "ERROR: devision by zero attempted in GravForce()! Throwing exception!" << std::endl;
            throw (DevByZero);
        }
        return (double(CONST_G) * mass1Kg * mass2Kg) / (distM * distM);
    }

    // Movement with constant acceleration
    void MoveWithConstAcc(Vector2d& pos, Vector2d& vel, const Vector2d& acc, const double& time)
    {
        // x = x0 + v0 * t + [(a * t^2) / 2]
        pos.x = pos.x + vel.x * time + (acc.x * time * time) / 2.0;
        pos.y = pos.y + vel.y * time + (acc.y * time * time) / 2.0;

        // v = v0 + [a * t]
        vel.x = vel.x + acc.x * time;
        vel.y = vel.y + acc.y * time;
    }

}; // namespace physics
