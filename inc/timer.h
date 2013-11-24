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
#include "thread_wrapper.h"
#include "mutex_wrapper.h"
}

class Timer {
    long int _interval_millisec;
    thread_func _timer_callback;
    void* _cb_param;
    volatile bool _running;
    bool _repeating;

    mutex_hdl start_stop_mutex;
    timeval _start_time;
    timeval _current_time;

    thread_hdl _wait_loop_thread;
    static thread_ret win_attr wait_loop(void* arg);
    static long int timeval_diff(const timeval& t1, const timeval& t2);

public:
    Timer(int interval_millisec,
          thread_func timer_callback,
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
    mutex_hdl start_stop_mutex;
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
    mutex_hdl reset_mutex;
public:
    FPSCounter();
    ~FPSCounter();
    void update_on_frame();
    void reset();
    float get_fps();
};

#endif /* defined(__timer__simplespace__) */
