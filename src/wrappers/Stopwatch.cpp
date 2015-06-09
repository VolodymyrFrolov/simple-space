//
//  Stopwatch.cpp
//
//  Created by Vladimir Frolov
//

#include "Stopwatch.h"


Stopwatch::Stopwatch(bool isStarted /* = true */) : mIsRunning(isStarted)
{
    wMutexInit(&mStateLock);
    if (mIsRunning) {
        wTimeNow(&mStartTime);
    } else {
        wTimeZero(&mStartTime);
    }
    mStopTime = mStartTime;
}


Stopwatch::~Stopwatch()
{
    wMutexDestroy(&mStateLock);
}


void Stopwatch::start()
{
    wMutexLock(&mStateLock);
    if (!mIsRunning)
    {
        mIsRunning = true;
        wTimeNow(&mStartTime);
        mStopTime = mStartTime;
    }
    wMutexUnlock(&mStateLock);
}


void Stopwatch::stop()
{
    wMutexLock(&mStateLock);
    if (mIsRunning)
    {
        mIsRunning = false;
        wTimeNow(&mStopTime);
    }
    wMutexUnlock(&mStateLock);
}


unsigned long Stopwatch::timeElaplsedMs() const
{
    if (mIsRunning)
    {
        wTime nowTime;
        wTimeNow(&nowTime);
        return wTimeDiffMs(&mStartTime, &nowTime);
    }
    else
    {
        return wTimeDiffMs(&mStartTime, &mStopTime);
    }
}

bool Stopwatch::isRunning() const
{
    return mIsRunning;
}
