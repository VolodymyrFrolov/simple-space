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
             bool started = true,
             bool repeating = true) :
    _interval_millisec(interval_millisec),
    _timer_callback(callback),
    _running(started),
    _repeating(repeating),
    _callback_running(false) {

    if (_interval_millisec <= 0)
        _running = false;

    if (_running)
        start();
}

Timer::~Timer() {
    // Show warning if timer is destroyed when callback is still running
    // Although callback is running in detached thread, so it should be finished
    if (_callback_running)
        cout << "Timer was destroyed while callback is still in progress" << endl;
}

long int Timer::timeval_diff(const timeval& t1, const timeval& t2) const {
    return (t2.tv_sec - t1.tv_sec) * 1000000 + (t2.tv_usec - t1.tv_usec);
}

void Timer::start() {
    std::thread(&Timer::wait_func, this).detach();
}

void Timer::stop() {
    cout << "timer stop()" << endl;
    _running = false;
}

void Timer::wait_func() {

    cout << "timer wait_func started" << endl;

    gettimeofday(&_start_time, NULL);
    _current_time = _start_time;

    do {
        while ((timeval_diff(_start_time, _current_time) < (_interval_millisec * 1000)) && _running) {
            std::this_thread::yield(); // Optional
            gettimeofday(&_current_time, NULL);
        }

        // Trigger callback
        gettimeofday(&_start_time, NULL);
        if (_timer_callback != NULL && _running == true) {
            _callback_running = true;
            this->_timer_callback();
            _callback_running = false;
        }
        gettimeofday(&_current_time, NULL);

        // Warn if callback took more time then interval
        if (timeval_diff(_start_time, _current_time) > (_interval_millisec * 1000))
            cout << "Warining! timer callback takes more time ("
                 << (_current_time.tv_sec - _start_time.tv_sec) * 1000000 + \
                     (_current_time.tv_usec - _start_time.tv_usec)
                 << " microseconds) than interval ("
                 << (_interval_millisec * 1000)
                 << " microseconds)" << endl;

    } while (_repeating && _running);

    cout << "timer wait_func finished" << endl;
}
