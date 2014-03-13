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

extern "C" {
#include "wrp_common.h"
#include "wrp_mutex.h"
#include "wrp_thread.h"
}

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <sys/time.h> // gettimeofday()
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

// ********** Common Functions **********
int wrp_time_now(wrp_time_t& time);
unsigned long wrp_time_to_ms(const wrp_time_t& time);
unsigned long wrp_time_diff_ms(const wrp_time_t& earlier, const wrp_time_t& later);

// ********** Timer **********
class Timer {
    wrp_thread_func_t _callback;
    void*             _param;
    unsigned long     _interval_ms;
    volatile bool     _running;
    bool              _repeating;

    wrp_mutex_t state_lock;
    wrp_thread_t loop_thread;
    static wrp_thread_ret_t win_attr loop_func(void* arg);

public:
    Timer(wrp_thread_func_t callback,
          void*             param,
          unsigned long     interval_ms,
          bool              started = true,
          bool              repeating = true);
    ~Timer();

    void start();
    void stop();
};

// ********** StopWatch **********
class StopWatch {
    bool _running;
    wrp_time_t start_time;
    wrp_time_t stop_time;
    wrp_mutex_t state_lock;
public:
    StopWatch(bool started = true);
    ~StopWatch();
    void start();
    void stop();
    unsigned long time_elaplsed_ms() const;
    bool running() const;
};

// ********** FPSCounter **********
class FPSCounter {
    float fps;
    unsigned int framecount;
    wrp_time_t start_time;
    wrp_mutex_t state_lock;
public:
    FPSCounter();
    ~FPSCounter();
    void update_on_frame();
    void reset();
    float get_fps();
};

#endif /* defined(__timer__simplespace__) */
