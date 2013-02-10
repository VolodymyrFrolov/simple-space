//
//  main.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 07.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include <iostream>
using std::cout;
using std::endl;

#include <stdlib.h> // For rand()
#include <time.h>   // For time()

#include "physics.h"
#include "planet.h"
#include "simplespace.h"

// Creating global SimpleSpace
SimpleSpace * pSimpleSpace = new SimpleSpace;

int main(int argc, const char * argv[])
{
    cout << "Simple-Space: main Stared" << endl;

    // Seed for random values
    srand ( (unsigned int)(time(NULL)) );


    // SimpleSpace testing begin
    double dist = 1e7;
    pSimpleSpace->addPlanet(Planet("P1", EARTH_MASS_KG, EARTH_RAD_M, Position(dist,dist), Velocity()));
    pSimpleSpace->addPlanet(Planet("P2", EARTH_MASS_KG, EARTH_RAD_M, Position(-dist,dist), Velocity()));
    pSimpleSpace->addPlanet(Planet("P3", EARTH_MASS_KG, EARTH_RAD_M, Position(-dist,-dist), Velocity()));
    for (int i = 0; i < 5; i++)
    {
        pSimpleSpace->MoveOneStep();
    }
    pSimpleSpace->ShowLogs();
    // SimpleSpace testing end


    // Delete global SimpleSpace
    delete pSimpleSpace;
    pSimpleSpace = NULL;
    cout << "Simple-Space: main Finished" << endl;
    return 0;
}
