//
//  main.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 07.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include <iostream>
#include "phys.h"
int main(int argc, const char * argv[])
{
    phys::Position<double> P = {};
    phys::Velocity<double> V = {};
    phys::Body<double> B1(P, V);
    // insert code here...
    std::cout << "Hello, Simple Space!\n";
    return 0;
}

