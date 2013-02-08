//
//  phys_templates.h
//  Satelite_console_v1
//
//  Created by Vladimir Frolov on 14.10.12.
//  Copyright (c) 2012 Vladimir Frolov. All rights reserved.
//

#ifndef Satelite_console_v1_phys_templates_h
#define Satelite_console_v1_phys_templates_h

#include <iostream>
#include <math.h>
using std::pair;
using std::string;

#define CONST_G 6.67385e-11
#define DevByZero -1

namespace phys_templates {


    // Position, Velocity and Acceleration structures
    template <class T>
    struct Position
    {
        Position(T coord_x = 0, T coord_y = 0) : x(coord_x), y(coord_y) {}
        T x;
        T y;
    };
    
    template <class T>
    struct Velocity
    {
        Velocity(T vel_x = 0, T vel_y = 0) : x(vel_x), y(vel_y) {}
        T x;
        T y;
    };
    
    template <class T>
    struct Acceleration
    {
        Acceleration(T accel_x = 0, T accel_y = 0) : x(accel_x), y(accel_y) {}
        T x;
        T y;
    };
    // Position, Velocity and Acceleration structures end




    // Body class
    template <class T>
    class Body
    {
    public:
        // Constructor
        Body(string name = "unnamed",
             Position<T> pos = Position<T>(),
             Velocity<T> vel = Velocity<T>());
        
        // Getters
        const string& Name() const;
        const Position<T>& Pos() const;
        const Velocity<T>& Vel() const;

    protected:
        // Private members for derived classes
        string _name;
        Position<T> _pos;
        Velocity<T> _vel;
    };
    
    // Constructor
    template <class T>
    Body<T>::Body(string name,
                  Position<T> pos,
                  Velocity<T> vel) : _name(name), _pos(pos), _vel(vel) {}
    
    // Public interface
    template <class T>
    const string& Body<T>::Name() const
    {
        return _name;
    }
    
    template <class T>
    const Position<T>& Body<T>::Pos() const
    {
        return _pos;
    }
    
    template <class T>
    const Velocity<T>& Body<T>::Vel() const
    {
        return _vel;
    }
    // Body class end



    
    // Angle conversions: Deg->Rad & Rad->Deg
    template <class T>
    T DegToRad(const T& Deg)
    {
        return ( Deg * T(M_PI) ) / T(180);
    }
    
    template <class T>
    T RadToDeg(const T& Rad)
    {
        return (Rad * 180) / T(M_PI);
    }
    
    
    // Hypotenoose: c = sqrt( a^2 + b^2 )
    template <class T>
    T Hypotenuse(const T& a, const T& b)
    {
        return sqrt( (a*a) + (b*b) );
    }
    
    
    // Distance by two points: x0,y0 and x1,y1
    template <class T>
    T DistFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        T tmpX = x1 - x0;
        T tmpY = y1 - y0;
        return Hypotenuse( tmpX, tmpY );
    }


    // Distance by two points: pos1, pos2
    template <class T>
    T DistFromPos(const Position<T>& pos0, const Position<T>& pos1)
    {
        return DistFromPos( pos0.x, pos0.y, pos1.x, pos1.y );
    }


    // Angle (in Radians) of line by two points: (x0,y0), (x1,y1)
    template <class T>
    T AngleFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        T tmpX = x1 - x0;
        T tmpY = y1 - y0;
        
        // Case of same position
        if ( (tmpX == 0) && (tmpY == 0) )
            return 0;
        
        if (tmpX < 0)
        {
            return ( T(M_PI) + atan(tmpY/tmpX) );
        }
        else
        {
            if (tmpY < 0)
                return (2 * T(M_PI) + atan(tmpY/tmpX));
            else
                return ( atan(tmpY/tmpX) );
        }
    }
    
    // Angle (in Radians) of line by two points: pos1, pos2
    template <class T>
    T AngleFromPos(const Position<T>& pos0, const Position<T>& pos1)
    {
        return AngleFromPos( pos0.x, pos0.y, pos1.x, pos1.y );
    }


    // Input: coordinates of two points (x0,y0; x1,y1)
    // Return: Distance and Angle (returned by pair)
    template <class T>
    pair<T,T> DistAngleFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        // Pair of return values:
        // .first = Distance; .second = Angle;
        pair<T,T> ret;

        // Distance
        ret.first = DistFromPos( x0, y0, x1, y1 );

        // Angle
        ret.second = AngleFromPos( x0, y0, x1, y1 );

        return ret;
    }
    
    
    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    template <class T>
    pair<T,T> DistAngleFromPos(const Position<T>& pos0, const Position<T>& pos1)
    {
        return DistAngleFromPos( pos0.x, pos0.y, pos1.x, pos1.y );
    }


    // G-force
    template <class T>
    T getGravAcc(const T& massKg, const T& distM)
    {
        if (distM == 0)
            throw (DevByZero);

        return ( ( T(CONST_G) * massKg) / (distM * distM) );
    }
    
    
    // Movement with constant acceleration
    template <class T>
    void moveOneStep(Position<T>& pos, Velocity<T>& vel, const Acceleration<T>& acc, const T& time)
    {
        // x = x0 + v0 * t + [(a * t^2) / 2]
        pos.x = pos.x + vel.x * time + (acc.x * time * time) / 2.0;
        pos.y = pos.y + vel.y * time + (acc.y * time * time) / 2.0;
        
        // v = v0 + [a * t]
        vel.x = vel.x + acc.x * time;
        vel.y = vel.y + acc.y * time;
    }

} // phys_templates

#endif