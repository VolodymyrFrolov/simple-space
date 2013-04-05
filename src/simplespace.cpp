//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"

SimpleSpace::SimpleSpace() : _time_step_ms(100) {}

void SimpleSpace::move_one_step()
{
    if (planets.size() == 0)
        cout << "MoveOneStep: no planets" << endl;
    
#if (ENABLE_BORDERS > 0)
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it)
    {
        if ( ((it->pos.x+it->radM)<LEFT_BORDER) || ((it->pos.x+it->radM)>RIGHT_BORDER) )
            it->vel.x = -it->vel.x;

        if ( ((it->pos.y+it->radM)>TOP_BORDER) || ((it->pos.y+it->radM)<BOTTOM_BORDER) )
            it->vel.y = -it->vel.y;
    }
#endif
    
    // Collision detection and resolving
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita)
    {
        for (vector<Planet>::iterator itb = ++planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb)
        {
            if (ita == itb)
                continue;

            double DistAB = physics::DistFromPos(ita->pos, itb->pos);

            // Resolving here
            if ( DistAB < (ita->radM + itb->radM) )
            {
                // Collision detected

                // 0. Get Angle between XY and NT (Normal-Tangential) coordinate systems
                double angle_AB = physics::AngleFromPos(ita->pos, itb->pos);

                // 1. Pull the bodies apart as they are overlapping (correlating with their masses)
                // from m1*s1 = m2*s2 and s = s1+s2 we get s1 = s*m2/(m1+m2); s2 = s*m1/(m1+m2)
                double pullDist = (ita->radM + itb->radM) - DistAB;
                double pullDistA = (pullDist * itb->massKg) / (ita->massKg + itb->massKg);
                double pullDistB = (pullDist * ita->massKg) / (ita->massKg + itb->massKg);
                cout << "MoveOneStep: Collision!!! Overlaped: " << pullDist << endl;

                // Translate and rotate vectors to NT
                physics::phys_vector AB = itb->pos;
                physics::phys_vector BA = ita->pos;
                physics::TranslateVector(AB, -ita->pos.x, -ita->pos.y);
                physics::TranslateVector(BA, -itb->pos.x, -itb->pos.y);
                physics::RotateVector(AB, -angle_AB);
                physics::RotateVector(BA, M_PI-angle_AB);

                // Move bodies appart, leaving Y components same, as we are in NT coordiantes
                AB.x += pullDistB;
                BA.x += pullDistA;

                // Rotate and translate vectors back to XY
                physics::RotateVector(AB, angle_AB);
                physics::RotateVector(BA, M_PI+angle_AB);
                physics::TranslateVector(AB, ita->pos.x, ita->pos.y);
                physics::TranslateVector(BA, itb->pos.x, itb->pos.y);
                
                // Immediately update positions to prevent dublicate collision detection
                ita->pos = ita->newPos = BA;
                itb->pos = itb->newPos = AB;

                // 2. Find new velocities after collision
                // V1, V2 - velocities of body1,2 before impact
                // U1, U2 - velocities of body1,2 after impact
                // Move velocities to NT coordinate system
                physics::phys_vector V1 = ita->vel;
                physics::phys_vector V2 = itb->vel;
                physics::RotateVector(V1, -angle_AB);
                physics::RotateVector(V2, -angle_AB);

                // Fromulas: bodies: a,b, velocities: Initial, Final. n - normal vector
                // vaf = ((e+1)*mb*(vbi)n + (vai)n*(ma – e*mb))/(ma + mb)
                // vbf = ((e+1)*ma*(vai)n – (vbi)n*(ma – e*mb))/(ma + mb)
                // Get velocities after collision (in NT coordinates)
                phys_vector U1, U2;
                U1.x = ((1+E_COEF)*itb->massKg*V2.x + V1.x*(ita->massKg - E_COEF*itb->massKg)) / (ita->massKg + itb->massKg);
                U1.y = V1.y;
                U2.x = ((1+E_COEF)*ita->massKg*V1.x - V2.x*(ita->massKg - E_COEF*itb->massKg)) / (ita->massKg + itb->massKg);
                U2.y = V2.y;

                // Move velocities back to XY coordinate system from NT
                physics::RotateVector( U1, angle_AB );
                physics::RotateVector( U2, angle_AB );
                ita->vel = U1;
                itb->vel = U2;
            } // end of Resolving here
        }
    } // End of Collision detection and resolving

    // Calculate new positions for planets
    // TODO: implement gravity forces calculation only for all combinations of planet pairs
    // to reduce calculations
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita)
    {
        // Calculate acceleration
        physics::phys_vector acc_curr;
#if (ENABLE_GRAVITY > 0)
        for (vector<Planet>::const_iterator itb = planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb)
        {
            if (ita != itb)
            {
                // Calculate acceleration for current planet (ita), produced by one of other Planets (itb)
                double acc_abs;
                pair<double, double> DistAngle = physics::DistAngleFromPos(ita->pos, itb->pos);
                acc_abs = physics::GravAcc(itb->massKg, DistAngle.first);
                acc_curr.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc_curr.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
#endif
        
        // Apply movement for time "_time_step_ms" to position "nextPos"
        MoveWithConstAcc(ita->newPos, ita->vel, acc_curr, (_time_step_ms/1000.0));
    }

    // Set all planets to new positions
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        it->pos = it->newPos;
    }
}

void SimpleSpace::add_planet(const Planet& newPlanet)
{
    planets.push_back(newPlanet);
    // TODO: Check if new added planet overlaps with existing
}
