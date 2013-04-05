//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"

SimpleSpace::SimpleSpace() : _timeStepMs(100) {}

void SimpleSpace::MoveOneStep()
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
    for (vector<Planet>::iterator it_A = planets.begin(), it_A_end = planets.end(); it_A != it_A_end; ++it_A)
    {
        for (vector<Planet>::iterator it_B = ++planets.begin(), it_B_end = planets.end(); it_B != it_B_end; ++it_B)
        {
            if (it_A == it_B)
                continue;

            double DistAB = physics::DistFromPos(it_A->pos, it_B->pos);

            // Resolving here
            if ( DistAB < (it_A->radM + it_B->radM) )
            {
                // Collision detected

                // 0. Get Angle between XY and NT (Normal-Tangential) coordinate systems
                double angle_AB = physics::AngleFromPos(it_A->pos, it_B->pos);

                // 1. Pull the bodies apart as they are overlapping (correlating with their masses)
                // from m1*s1 = m2*s2 and s = s1+s2 we get s1 = s*m2/(m1+m2); s2 = s*m1/(m1+m2)
                double pullDist = (it_A->radM + it_B->radM) - DistAB;
                double pullDistA = (pullDist * it_B->massKg) / (it_A->massKg + it_B->massKg);
                double pullDistB = (pullDist * it_A->massKg) / (it_A->massKg + it_B->massKg);
                cout << "MoveOneStep: Collision!!! Overlaped: " << pullDist << endl;

                // Translate and rotate vectors to NT
                physics::phys_vector AB = it_B->pos;
                physics::phys_vector BA = it_A->pos;
                physics::TranslateVector(AB, -it_A->pos.x, -it_A->pos.y);
                physics::TranslateVector(BA, -it_B->pos.x, -it_B->pos.y);
                physics::RotateVector(AB, -angle_AB);
                physics::RotateVector(BA, M_PI-angle_AB);

                // Move bodies appart, leaving Y components same, as we are in NT coordiantes
                AB.x += pullDistB;
                BA.x += pullDistA;

                // Rotate and translate vectors back to XY
                physics::RotateVector(AB, angle_AB);
                physics::RotateVector(BA, M_PI+angle_AB);
                physics::TranslateVector(AB, it_A->pos.x, it_A->pos.y);
                physics::TranslateVector(BA, it_B->pos.x, it_B->pos.y);
                
                // Immediately update positions to prevent dublicate collision detection
                it_A->pos = it_A->newPos = BA;
                it_B->pos = it_B->newPos = AB;

                // 2. Find new velocities after collision
                // V1, V2 - velocities of body1,2 before impact
                // U1, U2 - velocities of body1,2 after impact
                // Move velocities to NT coordinate system
                physics::phys_vector V1 = it_A->vel;
                physics::phys_vector V2 = it_B->vel;
                physics::RotateVector(V1, -angle_AB);
                physics::RotateVector(V2, -angle_AB);

                // Fromulas: bodies: a,b, velocities: Initial, Final. n - normal vector
                // vaf = ((e+1)*mb*(vbi)n + (vai)n*(ma – e*mb))/(ma + mb)
                // vbf = ((e+1)*ma*(vai)n – (vbi)n*(ma – e*mb))/(ma + mb)
                // Get velocities after collision (in NT coordinates)
                phys_vector U1, U2;
                U1.x = ((1+E_COEF)*it_B->massKg*V2.x + V1.x*(it_A->massKg - E_COEF*it_B->massKg)) / (it_A->massKg + it_B->massKg);
                U1.y = V1.y;
                U2.x = ((1+E_COEF)*it_A->massKg*V1.x - V2.x*(it_A->massKg - E_COEF*it_B->massKg)) / (it_A->massKg + it_B->massKg);
                U2.y = V2.y;

                // Move velocities back to XY coordinate system from NT
                physics::RotateVector( U1, angle_AB );
                physics::RotateVector( U2, angle_AB );
                it_A->vel = U1;
                it_B->vel = U2;
            } // end of Resolving here
        }
    } // End of Collision detection and resolving

    // Calculate new positions for planets
    // TODO: implement gravity forces calculation only for all combinations of planet pairs
    // to reduce calculations
    for (vector<Planet>::iterator it_A = planets.begin(), it_A_end = planets.end(); it_A != it_A_end; ++it_A)
    {
        // Calculate acceleration
        physics::phys_vector acc_curr;
#if (ENABLE_GRAVITY > 0)
        for (vector<Planet>::const_iterator it_B = planets.begin(), it_B_end = planets.end(); it_B != it_B_end; ++it_B)
        {
            if (it_A != it_B)
            {
                // Calculate acceleration for current planet (it_A), produced by one of other Planets (it_B)
                double acc_abs;
                pair<double, double> DistAngle = physics::DistAngleFromPos(it_A->pos, it_B->pos);
                acc_abs = physics::GravAcc(it_B->massKg, DistAngle.first);
                acc_curr.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc_curr.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
#endif
        
        // Apply movement for time "_timeStepMs" to position "nextPos"
        MoveWithConstAcc(it_A->newPos, it_A->vel, acc_curr, (_timeStepMs/1000.0));
    }

    // Set all planets to new positions
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        it->pos = it->newPos;
    }
}

void SimpleSpace::addPlanet(const Planet& newPlanet)
{
    planets.push_back(newPlanet);
    // TODO: Check if new added planet overlaps with existing
}
