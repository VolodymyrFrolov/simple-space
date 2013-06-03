//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"
#include <sstream>

SimpleSpace::SimpleSpace(int timestep_ms) : _timestep_ms(timestep_ms) {
    planet_id = 0;
    cout << "SimpleSpace instance created with _timestep_ms: " << get_model_timestep_ms() << endl;
}
SimpleSpace::~SimpleSpace() {
    cout << "SimpleSpace instance destroyed" << endl;
}
int SimpleSpace::get_model_timestep_ms() const {return _timestep_ms;}
void SimpleSpace::set_model_timestep_ms(int timestep_ms) {_timestep_ms = timestep_ms;}

void SimpleSpace::move_one_step() {
    std::lock_guard<std::mutex> guard(step_mutex);
    // Save current parameters to previous values, except acc
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        it->prev_pos = it->pos;
        it->prev_vel = it->vel;
    }

    // Calculate new positions for planets with or w/o gravity after movement
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita)
    {
        phys_vector acc;
        #if (ENABLE_GRAVITY > 0)
        for (vector<Planet>::const_iterator itb = planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita != itb) {
                // Calculate acceleration for planet (ita), produced by one of other planets (itb)
                double acc_abs;
                pair<double, double> DistAngle = physics::DistAngleFromPos(ita->pos, itb->pos);
                acc_abs = physics::GravAcc(itb->massKg, DistAngle.first);
                acc.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
        ita->acc = acc;
        #endif

        physics::MoveWithConstAcc(ita->pos, ita->vel, acc, (_timestep_ms/1000.0));
    }

    // Collision detection and resolving
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        for (vector<Planet>::iterator itb = ++planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita == itb)
                continue;

            double dist = physics::DistFromPos(ita->pos, itb->pos);
            double rad_sum = ita->radM + itb->radM;
            if (dist < rad_sum) {

                // Debug log
                cout << "Collision between: " << ita->name << " and " << itb->name << endl;

                // Make step back for both bodies
                ita->pos = ita->prev_pos;
                ita->vel = ita->prev_vel;
                itb->pos = itb->prev_pos;
                itb->vel = itb->prev_vel;

                // Angle betwwn bodies is also angle between XY and Normal-Tangential (NT) coordinates system
                double angle = physics::AngleFromPos(ita->pos, itb->pos);

                // V1, V2 - velocities of body1, body2 before impact
                phys_vector V1 = ita->vel;
                phys_vector V2 = itb->vel;

                // Move velocities to NT coordinate system
                physics::RotateVector(V1, -angle);
                physics::RotateVector(V2, -angle);

                // U1, U2 - velocities of body1, body2 after impact
                phys_vector U1, U2;

                // Bodies: (a), (b); velocities: initial (1), final (2)
                // va2 = (e+1)*mb*vb1 + va1(ma - e*mb)/(ma+mb)
                // vb2 = (e+1)*ma*va1 + vb1(mb - e*ma)/(ma+mb)
                // Get velocities after collision (in NT coordinates)
                U1.y = V1.y;
                U2.y = V2.y;
                U1.x = ((1 + COEF_RES) * itb->massKg * V2.x + V1.x * (ita->massKg - COEF_RES * itb->massKg)) / (ita->massKg + itb->massKg);
                U2.x = ((1 + COEF_RES) * ita->massKg * V1.x + V2.x * (itb->massKg - COEF_RES * ita->massKg)) / (ita->massKg + itb->massKg);
                // Same formula, just to check from wiki
                //U1.x = (ita->massKg * V1.x + itb->massKg * V2.x + itb->massKg * COEF_RES * (V2.x - V1.x)) / (ita->massKg + itb->massKg);
                //U2.x = (itb->massKg * V2.x + ita->massKg * V1.x + ita->massKg * COEF_RES * (V1.x - V2.x)) / (ita->massKg + itb->massKg);

                // Move velocities back to XY coordinate system from NT
                physics::RotateVector(U1, angle);
                physics::RotateVector(U2, angle);
                ita->vel = U1;
                itb->vel = U2;

                // Re-do a step after collision with new vel's for both bodies
                physics::MoveWithConstAcc(ita->pos, ita->vel, ita->acc, (_timestep_ms/1000.0));
                physics::MoveWithConstAcc(itb->pos, itb->vel, itb->acc, (_timestep_ms/1000.0));

                move_apart_if_needed(*ita, *itb);
            }
        }
    }

    // Check for border collision
    #if (ENABLE_BORDERS > 0)
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        resolve_border_collision(*it);
    }
    #endif
}

void SimpleSpace::resolve_border_collision(Planet& p) {
    if ((p.pos.x + p.radM) > RIGHT_BORDER) {
        // Make step back if possible
        if (p.pos != p.prev_pos)
            p.pos = p.prev_pos;
        // Move inside the border if pos is still out of border
        if ((p.pos.x + p.radM) > RIGHT_BORDER)
            p.pos.x = RIGHT_BORDER - p.radM;
        // Invert velocity in case it is opposite to border
        if (p.vel.x > 0)
            p.vel.x = -p.vel.x * COEF_RES;
    }

    if ((p.pos.x - p.radM) < LEFT_BORDER) {
        if (p.pos != p.prev_pos)
            p.pos = p.prev_pos;

        if ((p.pos.x - p.radM) < LEFT_BORDER)
            p.pos.x = LEFT_BORDER + p.radM;

        if (p.vel.x < 0)
            p.vel.x = -p.vel.x * COEF_RES;
    }

    if ((p.pos.y + p.radM) > TOP_BORDER) {
        if (p.pos != p.prev_pos)
            p.pos = p.prev_pos;

        if ((p.pos.y + p.radM) > TOP_BORDER)
            p.pos.y = TOP_BORDER - p.radM;

        if (p.vel.y > 0)
            p.vel.y = -p.vel.y * COEF_RES;
    }

    if ((p.pos.y - p.radM) < BOTTOM_BORDER) {
        if (p.pos != p.prev_pos)
            p.pos = p.prev_pos;

        if ((p.pos.y - p.radM) < BOTTOM_BORDER)
            p.pos.y = BOTTOM_BORDER + p.radM;

        if (p.vel.y < 0)
            p.vel.y = -p.vel.y * COEF_RES;
    }
}

void SimpleSpace::move_apart_if_needed(Planet& p1, Planet& p2) {
    // Move bodies apart (correlating with their masses) if they are overlapping
    // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
    // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
    double dist = physics::DistFromPos(p1.pos, p2.pos);
    double rad_sum = p1.radM + p2.radM;
    if (dist < rad_sum) {
        double angle = physics::AngleFromPos(p1.pos, p2.pos);
        double pull_dist = (rad_sum - dist);
        // Debug log
        cout << "Moving apart for: " << pull_dist << endl;
        double move_dist_1 = (pull_dist * p2.massKg) / (p1.massKg + p2.massKg);
        double move_dist_2 = (pull_dist * p1.massKg) / (p1.massKg + p2.massKg);
        p1.pos.x -= move_dist_1 * cos(angle);
        p1.pos.y -= move_dist_1 * sin(angle);
        p2.pos.x += move_dist_2 * cos(angle);
        p2.pos.y += move_dist_2 * sin(angle);
        
        /*
        Planet * parr[2] = {&p1, &p1};
        for (int i = 0; i < 2; ++i) {
            for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
                Planet * pit = &(*it);
                if (parr[i] != pit)
                    move_apart_if_needed(p1, *it);
            }
        }
         */
    }
}

void SimpleSpace::add_planet(const Planet& new_planet) {
    std::lock_guard<std::mutex> guard(step_mutex);
    planet_id++;
    Planet p = new_planet;
    resolve_border_collision(p);
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        move_apart_if_needed(p, *it);
    }
    planets.push_back(p);
}

void SimpleSpace::add_planet_by_Pos_and_Vel(const phys_vector& pos, const phys_vector& vel) {
    std::stringstream ss;
    ss << planet_id;
    SimpleSpace::add_planet(Planet(ss.str(), 1e29, 2e6, pos, vel));
}

void SimpleSpace::remove_all_objects() {
    std::lock_guard<std::mutex> guard(step_mutex);
    planet_id = 0;
    planets.clear();
}
