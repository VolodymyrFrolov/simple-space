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

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #define wrp_time_t timeval

#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #define wrp_time_t LARGE_INTEGER

#else
    #error "Unsupported platform"
#endif

class Timer {
    long int _interval_millisec;
    wrp_thread_func_t _timer_callback;
    void* _cb_param;
    volatile bool _running;
    bool _repeating;

    wrp_mutex_t start_stop_mutex;
    wrp_thread_t _wait_loop_thread;
    static wrp_thread_ret_t win_attr wait_loop(void* arg);

    static int wrp_time_now(wrp_time_t& time);
    static unsigned long wrp_time_to_ms(const wrp_time_t& time);
    static unsigned long wrp_time_diff_ms(const wrp_time_t& earlier, const wrp_time_t& later);

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
