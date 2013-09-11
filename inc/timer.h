//
//  timer.h
//  simple-space
//
//  Created by Vladimir Frolov on 10.09.13.
//  Copyright (c) 2013 Vladimir Frolov. All rights reserved.
//

#ifndef __controls_manager__simplespace__
#define __controls_manager__simplespace__

#include <iostream>
#include <thread>
#include <sys/time.h> // gettimeofday()
//#include <time.h>

typedef void (*TimerCallback)();

class Timer {
    long int _interval_millisec;
    TimerCallback _timer_callback;
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
          bool started,
          bool repeating);
    ~Timer();

    void start();
    void stop();
    void change_time_interval(long int interval_millisec);
};

#endif /* defined(__controls_manager__simplespace__) */
