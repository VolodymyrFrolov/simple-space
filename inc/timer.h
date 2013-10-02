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
#include <thread>
#include <sys/time.h> // gettimeofday()
//#include <time.h>

typedef void (*TimerCallback)(void* param);

class Timer {
    long int _interval_millisec;
    TimerCallback _timer_callback;
    void* _cb_param;
    volatile bool _running;
    bool _repeating;

    std::mutex start_stop_mutex;
    timeval _start_time;
    timeval _current_time;

    std::thread _wait_loop_thread;
    void wait_loop();
    long int timeval_diff(const timeval& t1, const timeval& t2) const;

public:
    Timer(int interval_millisec,
          TimerCallback timer_callback,
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
    std::mutex start_stop_mutex;
public:
    StopWatch(bool started = true);
    void start();
    void stop();
    long int time_elaplsed_usec() const;
    bool running() const;
};

class FPSCounter {
    float _fps;
    int _framecount;
    timeval _start;
    std::mutex reset_mutex;
public:
    FPSCounter();
    void update_on_frame();
    void reset();
    float get_fps();
};

#endif /* defined(__timer__simplespace__) */
