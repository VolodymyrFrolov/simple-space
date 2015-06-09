//
//  Timer.h
//
//  Created by Vladimir Frolov
//

#ifndef _TIMER_H_
#define _TIMER_H_

extern "C"
{
    #include "osWrappers.h"
}

class Timer
{
    wThreadFunc   mOnTimerCb;
    void*         mArg;
    unsigned long mIntervalMs;
    volatile bool mIsRunning;
    bool          mIsRepeating;

    wMutex  mStateLock;
    wEvent  mEvent;
    wThread mLoopThread;

    void loopFunc();

    static int staticWrapper(void* arg);

public:
    Timer(wThreadFunc   onTimerAction,
          void*         arg,
          unsigned long intervalMs,
          bool          isStarted = true,
          bool          isRepeating = true);
    ~Timer();

    void start();
    void stop();
    bool isRunning();
};

#endif /* defined(_TIMER_H_) */
