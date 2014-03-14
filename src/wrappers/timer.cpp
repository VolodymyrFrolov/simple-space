//
//  timer.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include <stdio.h>

#include "timer.h"

// ********** Common Functions **********

#if defined(__WIN32__)
static LARGE_INTEGER gFrequency.QuadPart = 0;

void init_freq() {
    if (gFrequency.QuadPart == 0) {
        int ret = QueryPerformanceFrequency(&gFrequency);
        assert(ret != 0);
    }
}
#endif

void wrp_time_now(wrp_time_t& time) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = gettimeofday(&time, NULL);
    assert(ret == 0);
    #elif defined(__WIN32__)
    int ret = QueryPerformanceCounter(&time);
    assert(ret != 0);
    #endif
}

unsigned long wrp_time_to_ms(const wrp_time_t& time) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    return time.tv_sec*1000 + time.tv_usec/1000;
    #elif defined(__WIN32__)
    return (time.QuadPart*1000) / gFrequency.QuadPart;
#endif
}

unsigned long wrp_time_diff_ms(const wrp_time_t& earlier, const wrp_time_t& later) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    return (later.tv_sec - earlier.tv_sec)*1000 + (later.tv_usec - earlier.tv_usec)/1000;
    #elif defined(__WIN32__)
    return ((later.QuadPart - earlier.QuadPart)*1000) / gFrequency.QuadPart; // *1e6 to get usec
    #endif
}


// ********** Timer **********

Timer::Timer(wrp_thread_func_t callback,
             void*             param,
             unsigned long     interval_ms,
             bool              started,
             bool              repeating) :
    _callback(callback),
    _param(param),
    _interval_ms(interval_ms),
    _running(started),
    _repeating(repeating) {

    #if defined(__WIN32__)
    init_freq();
    #endif

    wrp_mutex_init(&state_lock);
    if (_running) {
        _running = false; // start() needs _running to be false
        start();
    }
}

Timer::~Timer() {
    stop();
    wrp_mutex_destroy(&state_lock);
}

void Timer::start() {
    wrp_mutex_lock(&state_lock);

    if (!_running) {
        _running = true;
        wrp_thread_create(&loop_thread, Timer::loop_func, this, true);
    }
    wrp_mutex_unlock(&state_lock);
}

void Timer::stop() {
    wrp_mutex_lock(&state_lock);
    if (_running) {
        _running = false;
        wrp_thread_join(loop_thread, NULL);
    }
    wrp_mutex_unlock(&state_lock);
}

wrp_thread_ret_t win_attr Timer::loop_func(void* arg) {

    if (arg == NULL) {
        return (wrp_thread_ret_t)-1;
    }

    Timer* pTimer = static_cast<Timer*>(arg);
    
    // Check here, not constructor,
    // as currently no exception implemented in constructor
    if (pTimer->_callback == NULL) {
        pTimer->_running = false;
        return (wrp_thread_ret_t)-1;
    }

    wrp_time_t current, start;
    wrp_time_now(current);
    start = current;

    do {
        while ((wrp_time_diff_ms(start, current) < (pTimer->_interval_ms)) && pTimer->_running) {
            wrp_sleep_ms(10);
            wrp_time_now(current);
        }

        // Trigger callback and save time before and after
        wrp_time_now(start);
        if (pTimer->_running) {
            pTimer->_callback(pTimer->_param);
        }
        wrp_time_now(current);

        // Warn if callback took more time then interval
        if (wrp_time_diff_ms(start, current) > (pTimer->_interval_ms))
            printf("Timer: callback took more time, than interval\n");

    } while (pTimer->_running && pTimer->_repeating);

    // In case of non-repeating timer, update status
    // Don't put mutex to prevent deadlock. Race won't cause problem
    pTimer->_running = false;
    return 0;
}


// ********** StopWatch **********

StopWatch::StopWatch(bool started) : _running(started) {

    #if defined(__WIN32__)
    init_freq();
    #endif

    wrp_mutex_init(&state_lock);
    wrp_time_now(start_time);
    stop_time = start_time;
}

StopWatch::~StopWatch() {
    wrp_mutex_destroy(&state_lock);
}

void StopWatch::start() {
    wrp_mutex_lock(&state_lock);
    if (!_running) {
        _running = true;
        wrp_time_now(start_time);
        stop_time = start_time;
    }
    wrp_mutex_unlock(&state_lock);
}

void StopWatch::stop() {
    wrp_mutex_lock(&state_lock);
    if (_running) {
        _running = false;
        wrp_time_now(stop_time);
    }
    wrp_mutex_unlock(&state_lock);
}

unsigned long StopWatch::time_elaplsed_ms() const {
    if (_running) {
        wrp_time_t now;
        wrp_time_now(now);
        return wrp_time_diff_ms(start_time, now);
    } else {
        return wrp_time_diff_ms(start_time, stop_time);
    }
}

bool StopWatch::running() const {
    return _running;
}


// ********** FPSCounter **********

FPSCounter::FPSCounter() : fps(0), framecount(0) {
    wrp_mutex_init(&state_lock);
    wrp_time_now(start_time);
}

FPSCounter::~FPSCounter() {
    wrp_mutex_destroy(&state_lock);
}

void FPSCounter::update_on_frame() {

    wrp_mutex_lock(&state_lock);
    ++framecount;
    wrp_time_t now;
    wrp_time_now(now);

    unsigned long period_ms = wrp_time_diff_ms(start_time, now);
    if (period_ms > 1000) {
        fps = (framecount * 1000) / static_cast<float>(period_ms);
        framecount = 0;
        start_time = now;
    }
    wrp_mutex_unlock(&state_lock);
}

void FPSCounter::reset() {
    wrp_mutex_lock(&state_lock);
    fps = 0;
    framecount = 0;
    wrp_time_now(start_time);
    wrp_mutex_unlock(&state_lock);
}

float FPSCounter::get_fps() {
    return fps;
}
