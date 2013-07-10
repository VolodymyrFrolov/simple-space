//
//  simplespace.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 09.02.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "simplespace.h"
#include <sstream>

SimpleSpace::SimpleSpace(int time_step_ms) : _time_step_ms(time_step_ms) {
    planet_id = 0;
    cout << "SimpleSpace instance created with _timestep_ms: " << get_model_time_step_ms() << endl;
}
SimpleSpace::~SimpleSpace() {
    cout << "SimpleSpace instance destroyed" << endl;
}
int SimpleSpace::get_model_time_step_ms() const {return _time_step_ms;}
void SimpleSpace::set_model_time_step_ms(int time_step_ms) {_time_step_ms = time_step_ms;}

void SimpleSpace::move_one_step() {
    std::lock_guard<std::mutex> guard(step_mutex);

    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        // Fisrt: save current parameters to previous values, except acc
        ita->prev_pos = ita->pos;
        ita->prev_vel = ita->vel;  // prev_vel is currently not used

        // Second: calculate new positions for planets with/without gravity after movement
        phys_vector acc;
        #if (GRAVITY_ENABLED > 0)
        for (vector<Planet>::const_iterator itb = planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita != itb) {
                // Calculate acceleration for some planet (ita), produced by others one by one (itb)
                double acc_abs;
                pair<double, double> DistAngle = physics::DistAngleFromPos(ita->prev_pos, itb->prev_pos);
                acc_abs = physics::GravAcc(itb->massKg, DistAngle.first);
                acc.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
        ita->acc = acc;
        #endif

        #if (BORDERS_ENABLED > 0)
        acc.x += GLOBAL_GRAVITY_ACC_X;
        acc.y += GLOBAL_GRAVITY_ACC_Y;
        #endif

        // Third: make movement, updating position and velocity
        physics::MoveWithConstAcc(ita->pos, ita->vel, acc, (_time_step_ms/1000.0));
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

                move_apart_bodies(*ita, *itb);

                // Angle between bodies is also angle between XY and Normal-Tangential (NT) coordinates system
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
    if ((pl.pos.x + pl.radM) > RIGHT_BORDER) {
        if ((pl.pos.x + pl.radM) > RIGHT_BORDER)
            pl.pos.x = RIGHT_BORDER - pl.radM;
        if (pl.vel.x > 0)
            pl.vel.x = -pl.vel.x * COEF_RES;
        pl.vel.y *= BORDER_FRICTION;
    }

    if ((pl.pos.x - pl.radM) < LEFT_BORDER) {
        if ((pl.pos.x - pl.radM) < LEFT_BORDER)
            pl.pos.x = LEFT_BORDER + pl.radM;
        if (pl.vel.x < 0)
            pl.vel.x = -pl.vel.x * COEF_RES;
        pl.vel.y *= BORDER_FRICTION;
    }

    if ((pl.pos.y + pl.radM) > TOP_BORDER) {
        if ((pl.pos.y + pl.radM) > TOP_BORDER)
            pl.pos.y = TOP_BORDER - pl.radM;
        if (pl.vel.y > 0)
            pl.vel.y = -pl.vel.y * COEF_RES;
        pl.vel.x *= BORDER_FRICTION;
    }

    if ((pl.pos.y - pl.radM) < BOTTOM_BORDER) {
        if ((pl.pos.y - pl.radM) < BOTTOM_BORDER)
            pl.pos.y = BOTTOM_BORDER + pl.radM;
        if (pl.vel.y < 0)
            pl.vel.y = -pl.vel.y * COEF_RES;
        pl.vel.x *= BORDER_FRICTION;
    }
}

void SimpleSpace::move_apart_bodies(Planet& p1, Planet& p2) {
    // Move bodies apart if they are overlapping (correlating with their masses)
    // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
    // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
    double dist = physics::DistFromPos(p1.pos, p2.pos);
    double rad_sum = p1.radM + p2.radM;
    if (dist < rad_sum) {
        double pull_dist_abs = (rad_sum - dist) * 1.001;
        double pull_dist_1 = (pull_dist_abs * p2.massKg) / (p1.massKg + p2.massKg);
        double pull_dist_2 = (pull_dist_abs * p1.massKg) / (p1.massKg + p2.massKg);
        double angle = physics::AngleFromPos(p1.pos, p2.pos);
        p1.pos.x -= pull_dist_1 * cos(angle);
        p1.pos.y -= pull_dist_1 * sin(angle);
        p2.pos.x += pull_dist_2 * cos(angle);
        p2.pos.y += pull_dist_2 * sin(angle);

        // Debug logs
        cout << "pulling: dist=" << dist << " rad_sum=" << p1.radM + p2.radM << \
        " pull_dist_abs=" << pull_dist_abs << " pull_dist_1=" << pull_dist_1 << \
        " pull_dist_2=" << pull_dist_2 << endl;
    } else {
        // Debug logs
        cout << "WARNING!!!: pulling was not needed" << endl;
    }
}

void SimpleSpace::add_planet(const Planet& new_planet) {
    std::lock_guard<std::mutex> guard(step_mutex);
    planet_id++;
    Planet p = new_planet;
    resolve_border_collision(p);
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if (physics::DistFromPos(p.pos, it->pos) < (p.radM + it->radM))
            move_apart_bodies(p, *it);
    }
    planets.push_back(p);
}

void SimpleSpace::add_planet_by_Pos_and_Vel(const phys_vector& pos, const phys_vector& vel, double ang_vel, const ColorF_RGB col) {
    std::stringstream ss;
    ss << planet_id;
    SimpleSpace::add_planet(Planet(ss.str(), 1e29, 2e6, pos, vel, ang_vel, col));
}

void SimpleSpace::remove_all_objects() {
    std::lock_guard<std::mutex> guard(step_mutex);
    planet_id = 0;
    planets.clear();
}
