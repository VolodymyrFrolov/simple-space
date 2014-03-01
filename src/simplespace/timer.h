//
//  timer.h
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

// Classes:
//   Timer
//   StopWatch
//   FPSCounter

#ifndef __timer__simplespace__
#define __timer__simplespace__

#include <iostream>
#include <sys/time.h> // gettimeofday()
//#include <time.h>
extern "C" {
#include "wrp_mutex.h"
#include "wrp_thread.h"
}

class Timer {
    long int _interval_millisec;
    wrp_thread_func_t _timer_callback;
    void* _cb_param;
    volatile bool _running;
    bool _repeating;

    wrp_mutex_t start_stop_mutex;
    timeval _start_time;
    timeval _current_time;

    wrp_thread_t _wait_loop_thread;
    static wrp_thread_ret_t win_attr wait_loop(void* arg);
    static long int timeval_diff(const timeval& t1, const timeval& t2);

public:
    Timer(int interval_millisec,
          wrp_thread_func_t timer_callback,
          void* callback_param,
          bool started = true,
          bool repeating = true);
    ~Timer();

    void start();
    void stop();
    void change_interval(long int interval_millisec);
};

class StopWatch {
    timeval _start_time;
    timeval _stop_time;
    bool _running;
    wrp_mutex_t start_stop_mutex;
public:
    StopWatch(bool started = true);
    ~StopWatch();
    void start();
    void stop();
    long int time_elaplsed_usec() const;
    bool running() const;
};

class FPSCounter {
    float _fps;
    int _framecount;
    timeval _start;
    wrp_mutex_t reset_mutex;
public:
    FPSCounter();
    ~FPSCounter();
    void update_on_frame();
    void reset();
    float get_fps();
};

#endif /* defined(__timer__simplespace__) */
