//
//  timer.h
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

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
    volatile bool _callback_running;
    timeval _start_time;
    timeval _current_time;

    long int timeval_diff(const timeval& t1, const timeval& t2) const;
    void wait_func();

public:
    Timer(int interval_millisec,
          TimerCallback timer_callback,
          void* callback_param,
          bool started = true,
          bool repeating = true);
    ~Timer();

    void start();
    void stop();
};

#endif /* defined(__timer__simplespace__) */
