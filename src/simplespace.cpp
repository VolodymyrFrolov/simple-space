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
    pthread_mutex_init(&step_mutex, NULL);
    cout << "SimpleSpace instance created with _timestep_ms: " << get_model_timestep_ms() << endl;
}
SimpleSpace::~SimpleSpace() {
    pthread_mutex_destroy(&step_mutex);
    cout << "SimpleSpace instance destroyed" << endl;
}
int SimpleSpace::get_model_timestep_ms() const {return _timestep_ms;}
void SimpleSpace::set_model_timestep_ms(int timestep_ms) {_timestep_ms = timestep_ms;}

void SimpleSpace::move_one_step() {
    pthread_mutex_lock(&step_mutex);
    // Save current parameters to previous values, except acc
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        it->prev_pos = it->pos;
        it->prev_vel = it->pos;
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
                pair<double, double> DistAngle = physics::DistAngleFromPos(ita->prev_pos, itb->prev_pos);
                acc_abs = physics::GravAcc(itb->massKg, DistAngle.first);
                acc.x += acc_abs * cos(DistAngle.second);    // accX = acc * cos(fi)
                acc.y += acc_abs * sin(DistAngle.second);    // accY = acc * sin(fi)
            }
        }
        ita->prev_acc = acc;
        #endif

        physics::MoveWithConstAcc(ita->pos, ita->vel, acc, (_timestep_ms/1000.0));
    }

    // Collision detection and resolving
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = planets.end(); ita != ita_end; ++ita) {
        for (vector<Planet>::iterator itb = ++planets.begin(), itb_end = planets.end(); itb != itb_end; ++itb) {
            if (ita == itb)
                continue;

            double dist_ab = physics::DistFromPos(ita->pos, itb->pos);
            double rad_sum = ita->radM + itb->radM;
            if (dist_ab < rad_sum) {

                // Debug log
                cout << "Collision between: " << ita->name << " and " << itb->name << endl;

                // Make step back for both bodies
                ita->pos = ita->prev_pos;
                itb->pos = itb->prev_pos;

                // "angle_ab" is also angle between XY and Normal-Tangential (NT) coordinates system
                double angle_ab = physics::AngleFromPos(ita->pos, itb->pos);

                /*
                // Pull bodies apart (correlating with their masses) as they are overlapping
                // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
                // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
                // also make little addition to prevent dublicate collision detection
                double push_dist = (rad_sum - dist_ab);
                double push_dist_a = (push_dist * itb->massKg) / (ita->massKg + itb->massKg);
                double push_dist_b = (push_dist * ita->massKg) / (ita->massKg + itb->massKg);
                ita->pos.x -= push_dist_a * cos(angle_ab);
                ita->pos.y -= push_dist_a * sin(angle_ab);
                itb->pos.x += push_dist_b * cos(angle_ab);
                itb->pos.y += push_dist_b * sin(angle_ab);

                // Debug logs
                //cout.precision(10);
                //cout << "Collision detected! overlap: " << rad_sum - dist_ab
                //     << "; after-push-real-dist: " << physics::DistFromPos(ita->pos, itb->pos) - (ita->radM + itb->radM) << endl;
                */

                // Find new velocities after collision
                // V1, V2 - velocities of body1,2 before impact
                // U1, U2 - velocities of body1,2 after impact
                // Move velocities to NT coordinate system
                phys_vector V1 = ita->vel;
                phys_vector V2 = itb->vel;
                physics::RotateVector(V1, -angle_ab);
                physics::RotateVector(V2, -angle_ab);

                // Bodies: (a), (b); velocities: initial (1), final (2)
                // va2 = (e+1)*mb*vb1 + va1(ma - e*mb)/(ma+mb)
                // vb2 = (e+1)*ma*va1 + vb1(mb - e*ma)/(ma+mb)
                // Get velocities after collision (in NT coordinates)
                phys_vector U1, U2;

                U1.y = V1.y;
                U2.y = V2.y;
                U1.x = ((1 + COEF_RES) * itb->massKg * V2.x + V1.x * (ita->massKg - COEF_RES * itb->massKg)) / (ita->massKg + itb->massKg);
                U2.x = ((1 + COEF_RES) * ita->massKg * V1.x + V2.x * (itb->massKg - COEF_RES * ita->massKg)) / (ita->massKg + itb->massKg);
                // Same formula, just to check from wiki
                //U1.x = (ita->massKg * V1.x + itb->massKg * V2.x + itb->massKg * COEF_RES * (V2.x - V1.x)) / (ita->massKg + itb->massKg);
                //U2.x = (itb->massKg * V2.x + ita->massKg * V1.x + ita->massKg * COEF_RES * (V1.x - V2.x)) / (ita->massKg + itb->massKg);

                // Move velocities back to XY coordinate system from NT
                physics::RotateVector( U1, angle_ab );
                physics::RotateVector( U2, angle_ab );
                ita->vel = U1;
                itb->vel = U2;

                // re-make a step after collision for both bodies
                physics::MoveWithConstAcc(ita->pos, ita->vel, ita->prev_acc, (_timestep_ms/1000.0));
                physics::MoveWithConstAcc(itb->pos, ita->vel, itb->prev_acc, (_timestep_ms/1000.0));
            }
        }
    }

    // Check for border collision
    #if (ENABLE_BORDERS > 0)
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if ((it->pos.x + it->radM) > RIGHT_BORDER) {
            it->pos.x = RIGHT_BORDER - it->radM;
            if (it->vel.x > 0)
                it->vel.x = -it->vel.x * COEF_RES;
            //cout << "RIGHT_BORDER detected!" << endl;
        }
        if ((it->pos.x - it->radM) < LEFT_BORDER) {
            it->pos.x = LEFT_BORDER + it->radM;
            if (it->vel.x < 0)
                it->vel.x = -it->vel.x * COEF_RES;
            //cout << "LEFT_BORDER detected!" << endl;
        }
        if ((it->pos.y + it->radM) > TOP_BORDER) {
            it->pos.y = TOP_BORDER - it->radM;
            if (it->vel.y > 0)
                it->vel.y = -it->vel.y * COEF_RES;
            //cout << "TOP_BORDER detected!" << endl;
        }
        if((it->pos.y - it->radM) < BOTTOM_BORDER) {
            it->pos.y = BOTTOM_BORDER + it->radM;
            if (it->vel.y < 0)
                it->vel.y = -it->vel.y * COEF_RES;
            //cout << "BOTTOM_BORDER detected!" << endl;
        }
    }
    #endif

    pthread_mutex_unlock(&step_mutex);
}

void SimpleSpace::add_planet(const Planet& new_planet) {
    pthread_mutex_lock(&step_mutex);
    planet_id++;
    Planet p = new_planet;
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        pull_apart_planets(p, *it);
    }
    planets.push_back(p);
    pthread_mutex_unlock(&step_mutex);
}

void SimpleSpace::pull_apart_planets(Planet& p1, Planet& p2) {
    // Pull bodies apart (correlating with their masses) if they are overlapping
    // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
    // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
    double dist = physics::DistFromPos(p1.pos, p2.pos);
    double rad_sum = p1.radM + p2.radM;
    if (dist <= rad_sum) {
        double angle = physics::AngleFromPos(p1.pos, p2.pos);

        double pull_dist = (rad_sum - dist);
        double pull_dist_1 = (pull_dist * p2.massKg) / (p1.massKg + p2.massKg);
        double pull_dist_2 = (pull_dist * p1.massKg) / (p1.massKg + p2.massKg);
        p1.pos.x -= pull_dist_1 * cos(angle);
        p1.pos.y -= pull_dist_1 * sin(angle);
        p2.pos.x += pull_dist_2 * cos(angle);
        p2.pos.y += pull_dist_2 * sin(angle);
    }
}

void SimpleSpace::add_planet_by_Pos_and_Vel(const phys_vector& pos, const phys_vector& vel) {
    std::stringstream ss;
    ss << planet_id;
    SimpleSpace::add_planet(Planet(ss.str(), 1e29, 2e6, pos, vel));
}

void SimpleSpace::remove_all_objects() {
    planet_id = 0;
    planets.clear();
}
