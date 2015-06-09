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

const char* tag = "SimpleSpace";

SimpleSpace::SimpleSpace(int Time_Step_ms) : time_step_ms(Time_Step_ms), planets_number_max(500) {
    wMutexInit(&movement_step_mutex);
}
SimpleSpace::~SimpleSpace() {
    wMutexDestroy(&movement_step_mutex);
}

unsigned long SimpleSpace::get_planets_count() const {
    return planets.size();
}

int SimpleSpace::get_model_time_step_ms() const {
    return time_step_ms;
}

void SimpleSpace::move_one_step() {
    wMutexLock(&movement_step_mutex);

    if (planets.size() == 0)
        return;

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
    for (vector<Planet>::iterator ita = planets.begin(), ita_end = --planets.end(); ita != ita_end; ++ita) {
        for (vector<Planet>::iterator itb = ita + 1, itb_end = planets.end(); itb != itb_end; ++itb) {
            double dist = Physics::DistFromPos(ita->pos, itb->pos);
            double rad_sum = ita->rad_m + itb->rad_m;
            if (dist < rad_sum) {
                // Debug log
                //cout << "Collision between: " << ita->id << " and " << itb->id << endl;
                resolve_body_collision(*ita, *itb);
            }
        }
    }

    #if (BORDERS_ENABLED > 0)
    // Check for border collision
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        check_and_resolve_border_collision(*it);
    }
    #endif
    
    wMutexUnlock(&movement_step_mutex);
}

void SimpleSpace::move_apart_bodies(Planet& p1, Planet& p2) {
    // Move bodies apart (correlating with their masses)
    // from: d = d1 + d2; and: m1 * d1 = m2 * d2;
    // we get: d1 = d * m2 / (m1 + m2); d2 = d * m1 / (m1 + m2);
    double dist = Physics::DistFromPos(p1.pos, p2.pos);
    double rad_sum = p1.rad_m + p2.rad_m;

    double pull_dist_abs = (rad_sum - dist) * 1.001;
    double pull_dist_1 = (pull_dist_abs * p2.mass_kg) / (p1.mass_kg + p2.mass_kg);
    double pull_dist_2 = (pull_dist_abs * p1.mass_kg) / (p1.mass_kg + p2.mass_kg);
    double angle = Physics::AngleFromPos(p1.pos, p2.pos);
    p1.pos.x -= pull_dist_1 * cos(angle);
    p1.pos.y -= pull_dist_1 * sin(angle);
    p2.pos.x += pull_dist_2 * cos(angle);
    p2.pos.y += pull_dist_2 * sin(angle);

    // Debug logs
    //cout << "pulling: dist=" << dist << " rad_sum=" << p1.rad_m + p2.rad_m <<
    //" pull_dist_abs=" << pull_dist_abs << " pull_dist_1=" << pull_dist_1 <<
    //" pull_dist_2=" << pull_dist_2 << endl;
}

void SimpleSpace::resolve_body_collision(Planet& pla, Planet& plb) {
    move_apart_bodies(pla, plb);

    // Angle between bodies is also angle between XY and Normal-Tangential (NT) coordinates system
    double angle = Physics::AngleFromPos(pla.pos, plb.pos);

    // V1, V2 - velocities of body1, body2 before impact
    Vector2d V1 = pla.vel;
    Vector2d V2 = plb.vel;

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
    U1.x = ((1 + COEF_RES) * plb.mass_kg * V2.x + V1.x * (pla.mass_kg - COEF_RES * plb.mass_kg)) / (pla.mass_kg + plb.mass_kg);
    U2.x = ((1 + COEF_RES) * pla.mass_kg * V1.x + V2.x * (plb.mass_kg - COEF_RES * pla.mass_kg)) / (pla.mass_kg + plb.mass_kg);
    // Same formula, just to check from wiki
    //U1.x = (pla.mass_kg * V1.x + plb.mass_kg * V2.x + plb.mass_kg * COEF_RES * (V2.x - V1.x)) / (pla.mass_kg + plb.mass_kg);
    //U2.x = (plb.mass_kg * V2.x + pla.mass_kg * V1.x + pla.mass_kg * COEF_RES * (V1.x - V2.x)) / (pla.mass_kg + plb.mass_kg);

    // Move velocities back to XY coordinate system from NT
    Physics::RotateVector(U1, angle);
    Physics::RotateVector(U2, angle);
    pla.vel = U1;
    plb.vel = U2;
}

void SimpleSpace::check_and_resolve_border_collision(Planet& pl) {
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

void SimpleSpace::add_planet(const Planet& pl) {
    wMutexLock(&movement_step_mutex);

    unsigned int new_id = 0;
    while (std::any_of(planets.begin(), planets.end(), [&](Planet pl) {return pl.id == new_id;}))
        ++new_id;

    Planet new_planet = pl;
    new_planet.id = new_id;
    check_and_resolve_border_collision(new_planet);
    for (vector<Planet>::iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        double dist = Physics::DistFromPos(new_planet.pos, it->pos);
        double rad_sum = new_planet.rad_m + it->rad_m;
        if (dist < rad_sum)
            move_apart_bodies(new_planet, *it);
    }
    planets.push_back(new_planet);

    wMutexUnlock(&movement_step_mutex);
}

void SimpleSpace::remove_planet(const unsigned int& id) {
    wMutexLock(&movement_step_mutex);

    std::vector<Planet>::iterator it = std::find_if(planets.begin(), planets.end(), [&](Planet pl) {return pl.id == id;});
    if (it == planets.end()) {
        cout << "Didn't find planet to remove with id=" << id << endl;
    } else {
        planets.erase(it);
    }

    wMutexUnlock(&movement_step_mutex);
}

std::pair<bool, unsigned int> SimpleSpace::find_planet_by_click(const Vector2d& click_pos) {
    wMutexLock(&movement_step_mutex);

    pair<bool, unsigned int> result;
    result.first = false;
    result.second = std::numeric_limits<unsigned int>::max();
    for (vector<Planet>::const_iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if (Physics::DistFromPos(click_pos, it->pos) < it->rad_m) {
            result.first = true;
            result.second = it->id;
            break;
        }
    }

    wMutexUnlock(&movement_step_mutex);
    return result;
}

std::vector<unsigned int> SimpleSpace::find_planets_by_selection(const Vector2d& sel_start_pos,
                                                                 const Vector2d& sel_end_pos) {
    wMutexLock(&movement_step_mutex);
    double border_right  = (sel_end_pos.x > sel_start_pos.x) ? sel_end_pos.x : sel_start_pos.x;
    double border_top    = (sel_end_pos.y > sel_start_pos.y) ? sel_end_pos.y : sel_start_pos.y;
    double border_left   = (sel_end_pos.x > sel_start_pos.x) ? sel_start_pos.x : sel_end_pos.x;
    double border_bottom = (sel_end_pos.y > sel_start_pos.y) ? sel_start_pos.y : sel_end_pos.y;
    std::vector<unsigned int> found_id_list;
    for (vector<Planet>::const_iterator it = planets.begin(), it_end = planets.end(); it != it_end; ++it) {
        if ((it->pos.x < border_right) &&
            (it->pos.y < border_top) &&
            (it->pos.x > border_left) &&
            (it->pos.y > border_bottom)) {
            found_id_list.push_back(it->id);
        }
    }
    wMutexUnlock(&movement_step_mutex);
    return found_id_list;
}

void SimpleSpace::remove_all_objects() {
    wMutexLock(&movement_step_mutex);
    planets.clear();
    wMutexUnlock(&movement_step_mutex);
}

void SimpleSpace::draw_planet(const float& rad, const float& x, const float& y) const {
    glBegin(GL_POLYGON);
    float delta = M_PI / 50;
    for (float i = 0; i < 2 * M_PI; i += delta)
        glVertex2f(rad * cos(i) + x, rad * sin(i) + y);
    glEnd();
}

void SimpleSpace::draw_scene(const float& scale) const {
    std::for_each(planets.begin(), planets.end(), [&](Planet p) {
        glColor3f(p.color.R, p.color.G, p.color.B);
        draw_planet(p.rad_m/scale, p.pos.x/scale, p.pos.y/scale);
    } );
}
