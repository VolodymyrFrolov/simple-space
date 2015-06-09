//
//  Timer.cpp
//
//  Created by Vladimir Frolov
//

#include <stdexcept> // std::invalid_argument
#include "Timer.h"

Timer::Timer(wThreadFunc   onTimerCb,
             void*         arg,
             unsigned long intervalMs,
             bool          isStarted /* = true */,
             bool          isRepeating /* = true */) :
    mOnTimerCb(onTimerCb),
    mArg(arg),
    mIntervalMs(intervalMs),
    mIsRunning(isStarted),
    mIsRepeating(isRepeating)
{
    if (mOnTimerCb == NULL)
    {
        throw std::invalid_argument("Timer::Timer(): onTimerCb arg is NULL");
    }

    wMutexInit(&mStateLock);
    wEventInit(&mEvent);

    if (mIsRunning)
    {
        mIsRunning = false; // start() needs mIsRunning to be false
        this->start();
    }
}


Timer::~Timer()
{
    this->stop();
    wMutexDestroy(&mStateLock);
    wEventDestroy(&mEvent);
}


void Timer::start()
{
    wMutexLock(&mStateLock);
    if (!mIsRunning)
    {
        mIsRunning = true;
        if (wThreadCreate(&mLoopThread, Timer::staticWrapper, this, true) != 0) {
            // Error case when thread creation failed
            mIsRunning = false;
        }
    }
    wMutexUnlock(&mStateLock);
}


void Timer::stop()
{
    wMutexLock(&mStateLock);
    if (mIsRunning)
    {
        mIsRunning = false;
        wEventSignal(&mEvent);
        wThreadJoin(mLoopThread, NULL);
    }
    wMutexUnlock(&mStateLock);
}


bool Timer::isRunning()
{
    return mIsRunning;
}


int Timer::staticWrapper(void* arg)
{
    if (arg == NULL) {
        throw std::invalid_argument("Timer::staticWrapper(): arg is NULL");
    }

    // Starting timer loop
    (static_cast<Timer*>(arg))->loopFunc();

    return 0;
}


void Timer::loopFunc()
{
    unsigned long sleepLenMs = 0, callbackLenMs = 0;
    wTime cbEnterTime, cbExitTime;

    wTimeNow(&cbEnterTime);
    cbExitTime = cbEnterTime;

    while (mIsRunning)
    {
        callbackLenMs = wTimeDiffMs(&cbEnterTime, &cbExitTime);
        sleepLenMs = (callbackLenMs < mIntervalMs) ? mIntervalMs - callbackLenMs : 0;

        int waitRet = wEventWait(&mEvent, sleepLenMs);
        assert((waitRet == 0) || (waitRet == W_TIMEOUT_EXPIRED));
        if (waitRet != 0 && waitRet != W_TIMEOUT_EXPIRED) // Unexpected error
        {
            mIsRunning = false;
            break;
        }

        if (mIsRunning)
        {
            wTimeNow(&cbEnterTime);
            mOnTimerCb(mArg);
            wTimeNow(&cbExitTime);
        }

        // Warn if callback took more time than interval
        // if (wTimeDiffMs(&cbEnterTime, &cbExitTime) > mIntervalMs)
        // ...

        if (!mIsRepeating)
        {
            mIsRunning = false;
            break;
        }
    }
}
