//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"
#include <sstream>
#include <algorithm>
#include <vector>
using std::vector;

SimpleSpace::SimpleSpace(int Time_Step_ms) : time_step_ms(Time_Step_ms), planets_number_max(500) {
    cout << "SimpleSpace instance created with _timestep_ms: " << get_model_time_step_ms() << endl;
}
SimpleSpace::~SimpleSpace() {
    cout << "SimpleSpace instance destroyed" << endl;
}

vector<unsigned int> SimpleSpace::get_id_list() {
    vector<unsigned int> id_list;
    id_list.reserve(planets.size());
    for (vector<Planet>::const_iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        id_list.push_back(it->id);
    }
    return id_list;
}


int SimpleSpace::get_model_time_step_ms() const {return time_step_ms;}

void SimpleSpace::move_one_step() {
    std::lock_guard<std::mutex> guard(movement_step_mutex);

    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        // Fisrt: save current position
        ita->prev_pos = ita->pos;

        // Second: calculate new positions for planets with/without gravity after movement
        Vector2d acc;
        #if (GRAVITY_ENABLED > 0)
        for (vector<Planet>::const_iterator itb = planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita != itb) {
                // Calculate acceleration for some planet (ita), produced by others one by one (itb)
                double acc_abs;
                pair<double, double> DistAngle = Physics::DistAngleFromPos(ita->prev_pos, itb->prev_pos);
                acc_abs = Physics::GravAcc(itb->mass_kg, DistAngle.first);
                acc.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
        #endif

        #if (BORDERS_ENABLED > 0)
        acc.y += Physics::GravAcc(GLOBAL_TOP_MASS, abs(ita->pos.y - TOP_BORDER));
        acc.x += Physics::GravAcc(GLOBAL_RIGHT_MASS, abs(ita->pos.x - RIGHT_BORDER));
        #endif

        // Third: make movement, updating position and velocity
        Physics::MoveWithConstAcc(ita->pos, ita->vel, acc, (time_step_ms/1000.0));
    }

    // Collision detection and resolving
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        for (vector<Planet>::iterator itb = ++planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita == itb)
                continue;

            double dist = Physics::DistFromPos(ita->pos, itb->pos);
            double rad_sum = ita->rad_m + itb->rad_m;
            if (dist < rad_sum) {

                // Debug log
                cout << "Collision between: " << ita->id << " and " << itb->id << endl;

                move_apart_bodies(*ita, *itb);

                // Angle between bodies is also angle between XY and Normal-Tangential (NT) coordinates system
                double angle = Physics::AngleFromPos(ita->pos, itb->pos);

                // V1, V2 - velocities of body1, body2 before impact
                Vector2d V1 = ita->vel;
                Vector2d V2 = itb->vel;

                // Move velocities to NT coordinate system
                Physics::RotateVector(V1, -angle);
                Physics::RotateVector(V2, -angle);

                // U1, U2 - velocities of body1, body2 after impact
                Vector2d U1, U2;

                // Bodies: (a), (b); velocities: initial (1), final (2)
                // va2 = (e+1)*mb*vb1 + va1(ma - e*mb)/(ma+mb)
                // vb2 = (e+1)*ma*va1 + vb1(mb - e*ma)/(ma+mb)
                // Get velocities after collision (in NT coordinates)
                U1.y = V1.y;
                U2.y = V2.y;
                U1.x = ((1 + COEF_RES) * itb->mass_kg * V2.x + V1.x * (ita->mass_kg - COEF_RES * itb->mass_kg)) / (ita->mass_kg + itb->mass_kg);
                U2.x = ((1 + COEF_RES) * ita->mass_kg * V1.x + V2.x * (itb->mass_kg - COEF_RES * ita->mass_kg)) / (ita->mass_kg + itb->mass_kg);
                // Same formula, just to check from wiki
                //U1.x = (ita->mass_kg * V1.x + itb->mass_kg * V2.x + itb->mass_kg * COEF_RES * (V2.x - V1.x)) / (ita->mass_kg + itb->mass_kg);
                //U2.x = (itb->mass_kg * V2.x + ita->mass_kg * V1.x + ita->mass_kg * COEF_RES * (V1.x - V2.x)) / (ita->mass_kg + itb->mass_kg);

                // Move velocities back to XY coordinate system from NT
                Physics::RotateVector(U1, angle);
                Physics::RotateVector(U2, angle);
                ita->vel = U1;
                itb->vel = U2;
            }
        }
    }

    #if (BORDERS_ENABLED > 0)
    // Check for border collision
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        resolve_border_collision(*it);
    }
    #endif
}

void SimpleSpace::resolve_border_collision(Planet& pl) {
    if ((pl.pos.x + pl.rad_m) > RIGHT_BORDER) {
        if ((pl.pos.x + pl.rad_m) > RIGHT_BORDER)
            pl.pos.x = RIGHT_BORDER - pl.rad_m;
        if (pl.vel.x > 0)
            pl.vel.x = -pl.vel.x * COEF_RES;
        pl.vel.y *= BORDER_FRICTION;
    }

    if ((pl.pos.x - pl.rad_m) < LEFT_BORDER) {
        if ((pl.pos.x - pl.rad_m) < LEFT_BORDER)
            pl.pos.x = LEFT_BORDER + pl.rad_m;
        if (pl.vel.x < 0)
            pl.vel.x = -pl.vel.x * COEF_RES;
        pl.vel.y *= BORDER_FRICTION;
    }

    if ((pl.pos.y + pl.rad_m) > TOP_BORDER) {
        if ((pl.pos.y + pl.rad_m) > TOP_BORDER)
            pl.pos.y = TOP_BORDER - pl.rad_m;
        if (pl.vel.y > 0)
            pl.vel.y = -pl.vel.y * COEF_RES;
        pl.vel.x *= BORDER_FRICTION;
    }

    if ((pl.pos.y - pl.rad_m) < BOTTOM_BORDER) {
        if ((pl.pos.y - pl.rad_m) < BOTTOM_BORDER)
            pl.pos.y = BOTTOM_BORDER + pl.rad_m;
        if (pl.vel.y < 0)
            pl.vel.y = -pl.vel.y * COEF_RES;
        pl.vel.x *= BORDER_FRICTION;
    }
}

void SimpleSpace::move_apart_bodies(Planet& p1, Planet& p2) {
    // Move bodies apart if they are overlapping (correlating with their masses)
    // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
    // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
    double dist = Physics::DistFromPos(p1.pos, p2.pos);
    double rad_sum = p1.rad_m + p2.rad_m;
    if (dist < rad_sum) {
        double pull_dist_abs = (rad_sum - dist) * 1.001;
        double pull_dist_1 = (pull_dist_abs * p2.mass_kg) / (p1.mass_kg + p2.mass_kg);
        double pull_dist_2 = (pull_dist_abs * p1.mass_kg) / (p1.mass_kg + p2.mass_kg);
        double angle = Physics::AngleFromPos(p1.pos, p2.pos);
        p1.pos.x -= pull_dist_1 * cos(angle);
        p1.pos.y -= pull_dist_1 * sin(angle);
        p2.pos.x += pull_dist_2 * cos(angle);
        p2.pos.y += pull_dist_2 * sin(angle);

        // Debug logs
        cout << "pulling: dist=" << dist << " rad_sum=" << p1.rad_m + p2.rad_m << \
        " pull_dist_abs=" << pull_dist_abs << " pull_dist_1=" << pull_dist_1 << \
        " pull_dist_2=" << pull_dist_2 << endl;
    } else {
        // Debug logs
        cout << "WARNING!!!: pulling was not needed" << endl;
    }
}

void SimpleSpace::add_planet(const Planet& pl) {
    std::lock_guard<std::mutex> guard(movement_step_mutex);

    unsigned int new_id = 0;
    vector<unsigned int> id_list = get_id_list();
    while (std::any_of(id_list.begin(), id_list.end(), [=](unsigned int id) {return id == new_id;}) && (new_id < planets_number_max))
        ++new_id;

    Planet new_planet = pl;
    new_planet.id = new_id;
    resolve_border_collision(new_planet);
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if (Physics::DistFromPos(new_planet.pos, it->pos) < (new_planet.rad_m + it->rad_m))
            move_apart_bodies(new_planet, *it);
    }
    planets.push_back(new_planet);
}

bool SimpleSpace::remove_planet(const unsigned int& id) {
    bool id_found = false;
    std::lock_guard<std::mutex> guard(movement_step_mutex);
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if (it->id == id) {
            id_found = true;
            planets.erase(it);
        }
        continue;
    }
    return id_found;
}


void SimpleSpace::remove_all_objects() {
    std::lock_guard<std::mutex> guard(movement_step_mutex);
    planets.clear();
}
