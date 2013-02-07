//
//  phys.h
//  Satelite_console_v1
//
//  Created by Vladimir Frolov on 14.10.12.
//  Copyright (c) 2012 Vladimir Frolov. All rights reserved.
//

#ifndef Satelite_console_v1_phys_h
#define Satelite_console_v1_phys_h

#include <iostream>
#include <math.h>
using std::pair;
using std::string;

#define CONST_G 6.67385e-11

namespace phys {
    
    // Position, Velocity and Acceleration structures
    template <class T>
    struct Position
    {
        T x;
        T y;
    };
    
    template <class T>
    struct Velocity
    {
        T x;
        T y;
    };
    
    template <class T>
    struct Acceleration
    {
        T x;
        T y;
    };
    
    // Body class
    template <class T>
    class Body
    {
    public:
        // Constructor
        Body(Position<T> pos,
             Velocity<T> vel,
             string name = "unnamed");
        
        // Public interface
        string getName() const;
        Position<T> getPos() const;
        Velocity<T> getVel() const;

    protected:
        // Private members
        string _name;
        Position<T> _pos;
        Velocity<T> _vel;
    };
    
    // Constructor
    template <class T>
    Body<T>::Body(Position<T> pos,
                  Velocity<T> vel,
                  string name) : _name(name), _pos(pos), _vel(vel) {}
    
    // Public interface
    template <class T>
    string Body<T>::getName() const
    {
        return _name;
    }
    
    template <class T>
    Position<T> Body<T>::getPos() const
    {
        return _pos;
    }
    
    template <class T>
    Velocity<T> Body<T>::getVel() const
    {
        return _vel;
    }
    
    
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
    T getHypotenuse(const T& a, const T& b)
    {
        return sqrt( (a*a) + (b*b) );
    }

    // Input: coordinates of two points (x0,y0; x1,y1)
    // Return: Distance and Angle (returned by pair)
    template <class T>
    pair<T,T> DistAngleFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        // Pair of return values:
        // .first = Distance; .second = Angle;
        pair<T,T> ret;
        
        T tmpX = x1 - x0;
        T tmpY = y1 - y0;
        
        // Distance
        ret.first = sqrt( (tmpX * tmpX) + (tmpY * tmpY) );
        
        // Angle
        if (tmpX < 0)
        {
            ret.second = ( T(M_PI) + atan(tmpY/tmpX) );
        }
        else
        {
            if (tmpY > 0)
                ret.second = ( atan(tmpY/tmpX) );
            else
                ret.second = (2 * T(M_PI) + atan(tmpY/tmpX));
        }
        
        return ret;
    }
    
    
    // Input: coordinates of points (pos0, pos1)
    // Return: Distance and Angle (returned by pair)
    template <class T>
    pair<T,T> DistAngleFromPos(const Position<T>& pos0, const Position<T>& pos1)
    {
        // Pair of return values:
        // .first = Distance; .second = Angle;
        pair<T,T> ret;
        
        T tmpX = pos1.x - pos0.x;
        T tmpY = pos1.y - pos0.y;
        
        // Distance
        ret.first = sqrt( (tmpX * tmpX) + (tmpY * tmpY) );
        
        // Angle
        if (tmpX < 0)
        {
            ret.second = ( T(M_PI) + atan(tmpY/tmpX) );
        }
        else
        {
            if (tmpY > 0)
                ret.second = ( atan(tmpY/tmpX) );
            else
                ret.second = (2 * T(M_PI) + atan(tmpY/tmpX));
        }
        
        return ret;
    }


    // Functionality included by DistAngleFromPos()
#if (0)
    // Distance using x0,y0 and x1,y1
    template <class T>
    T DistFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        T tmpX = x1 - x0;
        T tmpY = y1 - y0;
        return sqrt( (tmpX * tmpX) + (tmpY * tmpY) );
    }
#endif


    // Functionality included by DistAngleFromPos()
#if (0)
    // Angle (in Radians) of line defined by two points: (x0,y0), (x1,y1)
    template <class T>
    T AngleFromPos(const T& x0, const T& y0, const T& x1, const T& y1)
    {
        T tmpX = x1 - x0;
        T tmpY = y1 - y0;
        if (tmpX < 0)
        {
            return ( T(M_PI) + atan(tmpY/tmpX) );
        }
        else
        {
            if (tmpY > 0)
                return ( atan(tmpY/tmpX) );
            else
                return (2 * T(M_PI) + atan(tmpY/tmpX));
        }
    }
#endif
    
    
    // G-force
    template <class T>
    T getGravAcc(const T& massKg, const T& distRM)
    {
        return ( ( T(CONST_G) * massKg) / (distRM * distRM) );
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
}

#endif
