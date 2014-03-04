//
//  timer.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "timer.h"
#include <unistd.h>
using std::cout;
using std::endl;

#if defined(__WIN32__)
extern LARGE_INTEGER gSystemFrequency;
#endif

Timer::Timer(int interval_millisec,
             wrp_thread_func_t timer_callback,
             void* callback_param,
             bool started,
             bool repeating) :
    _interval_millisec(interval_millisec),
    _timer_callback(timer_callback),
    _cb_param(callback_param),
    _running(started),
    _repeating(repeating) {

    // Actually mutex is not needed here, but in case of imaginable wiered situation,
    // where start() is called before constructor is executed
    wrp_mutex_init(&start_stop_mutex);
    wrp_mutex_lock(&start_stop_mutex);
    printf("Timer::Timer: _interval_millisec=%ld _running=%d\n", _interval_millisec, _running);

    if (_interval_millisec <= 0) {
        cout << "Error: [Timer] interval in constructor " << _interval_millisec << "<= 0" << endl;
        _running = false;
    } else if (_running) {
        wrp_thread_create(&_wait_loop_thread, Timer::wait_loop, this);
    }
    wrp_mutex_unlock(&start_stop_mutex);
}

Timer::~Timer() {
    stop();
    wrp_mutex_destroy(&start_stop_mutex);
}

int Timer::wrp_time_now(wrp_time_t& time) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = gettimeofday(&time, NULL);

    #elif defined(__WIN32__)
    if (QueryPerformanceCounter(&time) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("get_curr_time: Error\n");

    return ret;
}

static unsigned long wrp_time_to_ms(const wrp_time_t& time) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    return time.tv_sec*1000 + time.tv_usec/1000;

    #elif defined(__WIN32__)
    return (time.QuadPart*1000) / gSystemFrequency.QuadPart;
    #endif
}

unsigned long Timer::wrp_time_diff_ms(const wrp_time_t& earlier, const wrp_time_t& later) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    return (later.tv_sec - earlier.tv_sec)*1000 + (later.tv_usec - earlier.tv_usec)/1000;

    #elif defined(__WIN32__)
    return ((later.QuadPart - earlier.QuadPart)*1000) / gSystemFrequency.QuadPart;
    #endif
}

void Timer::start() {
    wrp_mutex_lock(&start_stop_mutex);

    printf("Timer::start\n");
    if (!_running && (_interval_millisec > 0)) {

        _running = true;
        wrp_thread_create(&_wait_loop_thread, Timer::wait_loop, this);
    }
    wrp_mutex_unlock(&start_stop_mutex);
}

void Timer::stop() {
    wrp_mutex_lock(&start_stop_mutex);
    if (_running) {
        _running = false;
        if (wrp_thread_join(_wait_loop_thread, NULL) != 0)
            printf("Timer::stop(): Error on joining thread\n");
    }
    wrp_mutex_unlock(&start_stop_mutex);
}

void Timer::change_interval(long int interval_millisec) {
    wrp_mutex_lock(&start_stop_mutex);

    if (interval_millisec > 0)
        _interval_millisec = interval_millisec;
    else
        cout << "Warning: [Timer] attempt to set interval to " << _interval_millisec << " was ignored" << endl;

     wrp_mutex_unlock(&start_stop_mutex);
}

wrp_thread_ret_t win_attr Timer::wait_loop(void* arg) {

    if (arg == NULL) {
        printf("Timer::wait_loop: Error: arg is NULL\n");
        return (void*)-1;
    }

    Timer* pTimer = static_cast<Timer*>(arg);
    
    // Check here, not constructor,
    // as currently no exception implemented in constructor
    if (pTimer->_timer_callback == NULL) {
        cout << "ERROR: [Timer] callback is NULL" << endl;
        pTimer->_running = false;
        return (void*)-10;
    }

    wrp_time_t current, start;
    wrp_time_now(current);
    start = current;

    do {
        while ((wrp_time_diff_ms(start, current) < (pTimer->_interval_millisec)) && pTimer->_running) {
            usleep(1000); // 1ms
            wrp_time_now(current);
        }

        // Trigger callback and save time before and after
        wrp_time_now(start);
        if (pTimer->_running) {
            pTimer->_timer_callback(pTimer->_cb_param);
        }
        wrp_time_now(current);

        // Warn if callback took more time then interval
        if (wrp_time_diff_ms(start, current) > (pTimer->_interval_millisec))
            cout << "Warning: [Timer] callback took more time, than interval" << endl;

    } while (pTimer->_running && pTimer->_repeating);

    // In case of non-repeating timer, update status
    // Don't put mutex to prevent deadlock. Race won't cause problem
    pTimer->_running = false;
    return 0;
}

StopWatch::StopWatch(bool started) : _running(started) {
    wrp_mutex_init(&start_stop_mutex);
    gettimeofday(&_start_time, NULL);
    _stop_time = _start_time;
}

StopWatch::~StopWatch() {
    wrp_mutex_destroy(&start_stop_mutex);
}

void StopWatch::start() {
    wrp_mutex_lock(&start_stop_mutex);
    if (!_running) {
        _running = true;
        gettimeofday(&_start_time, NULL);
        _stop_time = _start_time;
    }
    wrp_mutex_unlock(&start_stop_mutex);
}

void StopWatch::stop() {
    wrp_mutex_lock(&start_stop_mutex);
    if (_running) {
        _running = false;
        gettimeofday(&_stop_time, NULL);
    }
    wrp_mutex_unlock(&start_stop_mutex);
}

long int StopWatch::time_elaplsed_usec() const {
    if (_running) {
        timeval now;
        gettimeofday(&now, NULL);
        return (now.tv_sec - _start_time.tv_sec) * 1000000 + (now.tv_usec - _start_time.tv_usec);
    } else {
        return (_stop_time.tv_sec - _start_time.tv_sec) * 1000000 + (_stop_time.tv_usec - _start_time.tv_usec);
    }
}

bool StopWatch::running() const {
    return _running;
}


FPSCounter::FPSCounter() : _fps(0), _framecount(0) {
    wrp_mutex_init(&reset_mutex);
    gettimeofday(&_start, NULL);
}

FPSCounter::~FPSCounter() {
    wrp_mutex_destroy(&reset_mutex);
}

void FPSCounter::update_on_frame() {

    wrp_mutex_lock(&reset_mutex);
    ++_framecount;
    timeval now;
    gettimeofday(&now, NULL);

    long int period_usec = (now.tv_sec - _start.tv_sec) * 1000000 + (now.tv_usec - _start.tv_usec);
    if (period_usec > 1000000) {
        _fps = _framecount / (period_usec / 1000000.0f);
        _framecount = 0;
        _start = now;
    }
    wrp_mutex_unlock(&reset_mutex);
}

void FPSCounter::reset() {
    wrp_mutex_lock(&reset_mutex);
    _fps = 0;
    _framecount = 0;
    gettimeofday(&_start, NULL);
    wrp_mutex_unlock(&reset_mutex);
}

float FPSCounter::get_fps() {
    return _fps;
}
