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


class A {
public:
    thread_ret win_attr testfunc(void* arg) {
        return (void*)0;
    }

};

Timer::Timer(int interval_millisec,
             thread_func callback,
             void* callback_param,
             bool started,
             bool repeating) :
    _interval_millisec(interval_millisec),
    _timer_callback(callback),
    _cb_param(callback_param),
    _running(started),
    _repeating(repeating) {

    // Actually mutex is not needed here, but in case of imaginable wiered situation,
    // where start() is called before constructor is executed
    mutex_init_wrapper(&start_stop_mutex);
    mutex_lock_wrapper(&start_stop_mutex);
    printf("Timer::Timer: _interval_millisec=%ld _running=%d\n", _interval_millisec, _running);

    if (_interval_millisec <= 0) {
        cout << "Error: [Timer] interval in constructor " << _interval_millisec << "<= 0" << endl;
        _running = false;
    } else if (_running) {
        thread_create_wrapper(&_wait_loop_thread, Timer::wait_loop, this);
    }
    mutex_unlock_wrapper(&start_stop_mutex);
}

Timer::~Timer() {
    stop();
    mutex_deinit_wrapper(&start_stop_mutex);
}

long int Timer::timeval_diff(const timeval& t1, const timeval& t2) {
    return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

void Timer::start() {
    mutex_lock_wrapper(&start_stop_mutex);

    printf("Timer::start\n");
    if (!_running && (_interval_millisec > 0)) {

        _running = true;
        thread_create_wrapper(&_wait_loop_thread, Timer::wait_loop, this);
    }
    mutex_unlock_wrapper(&start_stop_mutex);
}

void Timer::stop() {
    mutex_lock_wrapper(&start_stop_mutex);
    if (_running) {
        _running = false;
        if (thread_join_wrapper(_wait_loop_thread, NULL) != 0)
            printf("Timer::stop(): Error on joining thread\n");
    }
    mutex_unlock_wrapper(&start_stop_mutex);
}

void Timer::change_interval(long int interval_millisec) {
    mutex_lock_wrapper(&start_stop_mutex);

    if (interval_millisec > 0)
        _interval_millisec = interval_millisec;
    else
        cout << "Warning: [Timer] attempt to set interval to " << _interval_millisec << " was ignored" << endl;

     mutex_unlock_wrapper(&start_stop_mutex);
}

thread_ret win_attr Timer::wait_loop(void* arg) {

    if(arg == NULL) {
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

    gettimeofday(&pTimer->_start_time, NULL);
    pTimer->_current_time = pTimer->_start_time;

    do {
        while ((Timer::timeval_diff(pTimer->_start_time, pTimer->_current_time) < (pTimer->_interval_millisec * 1000)) && pTimer->_running) {
            usleep(1000); // 1ms
            gettimeofday(&pTimer->_current_time, NULL);
        }

        // Trigger callback
        gettimeofday(&pTimer->_start_time, NULL);
        if (pTimer->_running) {
            pTimer->_timer_callback(pTimer->_cb_param);
        }
        gettimeofday(&pTimer->_current_time, NULL);

        // Warn if callback took more time then interval
        if (timeval_diff(pTimer->_start_time, pTimer->_current_time) > (pTimer->_interval_millisec * 1000))
            cout << "Warning: [Timer] callback took more time ("
                 << (pTimer->_current_time.tv_sec - pTimer->_start_time.tv_sec) * 1000000 + \
                     (pTimer->_current_time.tv_usec - pTimer->_start_time.tv_usec)
                 << " usec) than interval ("
                 << (pTimer->_interval_millisec * 1000)
                 << " usec)" << endl;

    } while (pTimer->_running && pTimer->_repeating);

    // In case of non-repeating timer, update status
    // Don't put mutex to prevent deadlock. Race won't cause problem
    pTimer->_running = false;
    return 0;
}

// Code example to add For Windows support:
/*
#ifdef WIN32
    LARGE_INTEGER frequency;                    // ticks per second
    LARGE_INTEGER startCount;                   //
    LARGE_INTEGER endCount;                     //
#else
    ...
#endif

#ifdef WIN32
    QueryPerformanceFrequency(&frequency);
    startCount.QuadPart = 0;
    endCount.QuadPart = 0;
#else
    ...
#endif

#ifdef WIN32
    QueryPerformanceCounter(&endCount);
#else
    ...
#endif

#ifdef WIN32
    if(!stopped)
        QueryPerformanceCounter(&endCount);

    startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
    endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
#else
    ...
#endif
*/

StopWatch::StopWatch(bool started) : _running(started) {
    mutex_init_wrapper(&start_stop_mutex);
    gettimeofday(&_start_time, NULL);
    _stop_time = _start_time;
}

StopWatch::~StopWatch() {
    mutex_deinit_wrapper(&start_stop_mutex);
}

void StopWatch::start() {
    mutex_lock_wrapper(&start_stop_mutex);
    if (!_running) {
        _running = true;
        gettimeofday(&_start_time, NULL);
        _stop_time = _start_time;
    }
    mutex_unlock_wrapper(&start_stop_mutex);
}

void StopWatch::stop() {
    mutex_lock_wrapper(&start_stop_mutex);
    if (_running) {
        _running = false;
        gettimeofday(&_stop_time, NULL);
    }
    mutex_unlock_wrapper(&start_stop_mutex);
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
    mutex_init_wrapper(&reset_mutex);
    gettimeofday(&_start, NULL);
}

FPSCounter::~FPSCounter() {
    mutex_deinit_wrapper(&reset_mutex);
}

void FPSCounter::update_on_frame() {

    mutex_lock_wrapper(&reset_mutex);
    ++_framecount;
    timeval now;
    gettimeofday(&now, NULL);

    long int period_usec = (now.tv_sec - _start.tv_sec) * 1000000 + (now.tv_usec - _start.tv_usec);
    if (period_usec > 1000000) {
        _fps = _framecount / (period_usec / 1000000.0f);
        _framecount = 0;
        _start = now;
    }
    mutex_unlock_wrapper(&reset_mutex);
}

void FPSCounter::reset() {
    mutex_lock_wrapper(&reset_mutex);
    _fps = 0;
    _framecount = 0;
    gettimeofday(&_start, NULL);
    mutex_unlock_wrapper(&reset_mutex);
}

float FPSCounter::get_fps() {
    return _fps;
}
