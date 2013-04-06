//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"

SimpleSpace::SimpleSpace() : _time_step_ms(100) {}

void SimpleSpace::move_one_step() {
    // Debug logs
    if (planets.size() == 0)
        cout << "MoveOneStep: no planets" << endl;
    
#if (ENABLE_BORDERS > 0)
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if (((it->pos.x - it->radM) < LEFT_BORDER) || ((it->pos.x + it->radM) > RIGHT_BORDER))
            it->vel.x = -it->vel.x;
        if (((it->pos.y + it->radM) > TOP_BORDER)  || ((it->pos.y - it->radM) < BOTTOM_BORDER))
            it->vel.y = -it->vel.y;
    }
#endif
    
    // Collision detection and resolving
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        for (vector<Planet>::iterator itb = ++planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita == itb)
                continue;

            double dist_ab = physics::DistFromPos(ita->pos, itb->pos);
            double rad_sum = ita->radM + itb->radM;
            if (dist_ab < rad_sum) {
                // "angle_ab" is also angle between XY and Normal-Tangential (NT) coordinates system
                double angle_ab = physics::AngleFromPos(ita->pos, itb->pos);

                // Pull bodies apart (correlating with their masses) as they are overlapping
                // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
                // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
                // also make little addition to prevent dublicate collision detection
                double push_dist = (rad_sum - dist_ab) + 0.1;
                double push_dist_a = (push_dist * itb->massKg) / (ita->massKg + itb->massKg);
                double push_dist_b = (push_dist * ita->massKg) / (ita->massKg + itb->massKg);
                ita->newPos.x -= push_dist_a * cos(angle_ab);
                ita->newPos.y -= push_dist_a * sin(angle_ab);
                itb->newPos.x += push_dist_b * cos(angle_ab);
                itb->newPos.y += push_dist_b * sin(angle_ab);

                // Immediately update positions and distance between bodies
                ita->pos = ita->newPos;
                itb->pos = itb->newPos;
                
                // Debug logs
                cout.precision(10);
                cout << "Collision detected! overlap: " << rad_sum - dist_ab
                     << "; after-push-real-dist: " << physics::DistFromPos(ita->pos, itb->pos) - (ita->radM + itb->radM) << endl;

                // Find new velocities after collision
                // V1, V2 - velocities of body1,2 before impact
                // U1, U2 - velocities of body1,2 after impact
                // Move velocities to NT coordinate system
                phys_vector V1 = ita->vel;
                phys_vector V2 = itb->vel;
                physics::RotateVector(V1, -angle_ab);
                physics::RotateVector(V2, -angle_ab);

                // Bodies: (a), (b); velocities: initial (i), final (f); n - normal vector
                // vaf = ((e+1)*mb*(vbi)n + (vai)n*(ma – e*mb))/(ma + mb)
                // vbf = ((e+1)*ma*(vai)n – (vbi)n*(ma – e*mb))/(ma + mb)
                // Get velocities after collision (in NT coordinates)
                phys_vector U1, U2;
                U1.x = ((1 + E_COEF) * itb->massKg * V2.x + V1.x * (ita->massKg - E_COEF * itb->massKg)) / (ita->massKg + itb->massKg);
                U1.y = V1.y;
                U2.x = ((1 + E_COEF) * ita->massKg * V1.x - V2.x * (ita->massKg - E_COEF * itb->massKg)) / (ita->massKg + itb->massKg);
                U2.y = V2.y;

                // Move velocities back to XY coordinate system from NT
                physics::RotateVector( U1, angle_ab );
                physics::RotateVector( U2, angle_ab );
                ita->vel = U1;
                itb->vel = U2;
            }
        }
    } // End of Collision detection and resolving

    // Calculate new positions for planets
    // TODO: decide wether need to implement gravity forces calculation
    // only for all combinations of planet pairs to reduce calculations
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita)
    {
        // Calculate acceleration
        phys_vector acc_curr;
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
        physics::MoveWithConstAcc(ita->newPos, ita->vel, acc_curr, (_time_step_ms/1000.0));
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
