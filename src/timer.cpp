//
//  timer.cpp
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#include "timer.h"
using std::cout;
using std::endl;

Timer::Timer(int interval_millisec,
             TimerCallback callback,
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
    std::lock_guard<std::mutex> guard(start_stop_mutex);

    if (_interval_millisec <= 0) {
        cout << "Error: [Timer] interval in constructor " << _interval_millisec << "<= 0" << endl;
        if (_running)
            _running = false;
    } else if (_running && !_wait_loop_thread.joinable()) {
        _wait_loop_thread = std::thread(&Timer::wait_loop, this);
    }
}

Timer::~Timer() {
    stop();
}

long int Timer::timeval_diff(const timeval& t1, const timeval& t2) const {
    return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

void Timer::start() {
    std::lock_guard<std::mutex> guard(start_stop_mutex);

    if (!_running && (_interval_millisec > 0)) {

        if (_wait_loop_thread.joinable())
            _wait_loop_thread.join();

        _running = true;
        _wait_loop_thread = std::thread(&Timer::wait_loop, this);
    }
}

void Timer::stop() {
    std::lock_guard<std::mutex> guard(start_stop_mutex);

    if (_running)
        _running = false;

    if (_wait_loop_thread.joinable())
       _wait_loop_thread.join();
}

void Timer::change_interval(long int interval_millisec) {
    std::lock_guard<std::mutex> guard(start_stop_mutex);

    if (interval_millisec > 0)
        _interval_millisec = interval_millisec;
    else
        cout << "Warning: [Timer] attempt to set interval to " << _interval_millisec << " was ignored" << endl;
}

void Timer::wait_loop() {

    // Check here, not constructor,
    // as currently no exception implemented in constructor
    if (_timer_callback == NULL) {
        cout << "ERROR: [Timer] callback is NULL" << endl;
        _running = false;
        return;
    }

    gettimeofday(&_start_time, NULL);
    _current_time = _start_time;

    do {
        while ((timeval_diff(_start_time, _current_time) < (_interval_millisec * 1000)) && _running) {
            std::this_thread::yield(); // Optional
            gettimeofday(&_current_time, NULL);
        }

        // Trigger callback
        gettimeofday(&_start_time, NULL);
        if (_running) {
            _timer_callback(_cb_param);
        }
        gettimeofday(&_current_time, NULL);

        // Warn if callback took more time then interval
        if (timeval_diff(_start_time, _current_time) > (_interval_millisec * 1000))
            cout << "Warning: [Timer] callback took more time ("
                 << (_current_time.tv_sec - _start_time.tv_sec) * 1000000 + \
                     (_current_time.tv_usec - _start_time.tv_usec)
                 << " microseconds) than interval ("
                 << (_interval_millisec * 1000)
                 << " microseconds)" << endl;

    } while (_running && _repeating);

    // In case of non-repeating timer, update status
    // Don't put mutex to prevent deadlock. Race won't cause problem
    _running = false;
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
