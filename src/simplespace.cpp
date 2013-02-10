//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"

SimpleSpace::SimpleSpace() : _timeStepMs(100) {}

// temp
void SimpleSpace::ShowLogs()
{
    if (PlanetList.size() == 0) {
        cout << "ShowLogs: No planets" << endl;
    } else {
        for (list<Planet>::const_iterator it = PlanetList.begin(), it_end = PlanetList.end(); it != it_end; ++it)
        cout << it->name << " X:" << it->pos.x << " Y:" << it->pos.y << endl;
    }
}


void SimpleSpace::MoveOneStep()
{
    if (PlanetList.size() == 0) {
        cout << "MoveOneStep: planets" << endl;
    }

    // Calculating new positions for planets
    for (list<Planet>::iterator it_A = PlanetList.begin(), it_A_end = PlanetList.end(); it_A != it_A_end; ++it_A)
    {
        // Calculating acceleration
        Acceleration acc_curr;
        for (list<Planet>::const_iterator it_B = PlanetList.begin(), it_B_end = PlanetList.end(); it_B != it_B_end; ++it_B)
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
        
        // Applying movement for time "_timeStepMs" to position "nextPos"
        MoveWithConstAcc(it_A->newPos, it_A->vel, acc_curr, (_timeStepMs/1000.0));
    }
    
    // Collision detection
    /*
    for (list<Planet>::iterator it_A = PlanetList.begin(), it_A_end = PlanetList.end(); it_A != it_A_end; ++it_A)
    {
        for (list<Planet>::const_iterator it_B = PlanetList.begin(), it_B_end = PlanetList.end(); it_B != it_B_end; ++it_B)
        {
            if (it_A != it_B)
            {
                if (physics::DistFromPos(it_A->newPos, it_B->pos) < (it_A->radM + it_B->radM)) {
                    cout << "MoveOneStep: Collision detected!!!" << endl;
                } else {
                    cout << "OK" << endl;
                }
            }
        }
    }
    */
    
    // Set all planets to new positions
    for (list<Planet>::iterator it = PlanetList.begin(), it_end = PlanetList.end(); it != it_end; ++it) {
        it->pos = it->newPos;
    }
}


void SimpleSpace::addPlanet()
{
    // generate random position
    physics::Position randPos;
    
    randPos.x = (rand() % 50) * 1e6; // 0 - 49'000 km
    randPos.y = (rand() % 50) * 1e6; // 0 - 49'000 km
    
    PlanetList.push_back(Planet("Random", EARTH_MASS_KG, EARTH_RAD_M, randPos, Velocity()));
}


void SimpleSpace::addPlanet(const Planet& newPlanet)
{
    PlanetList.push_back(newPlanet);
}