//
//  FpsCounter.cpp
//
//  Created by Vladimir Frolov
//

#include "FpsCounter.h"

#define MS_IN_SEC 1000

FpsCounter::FpsCounter() : mFps(0)
{
    wMutexInit(&mFrameLock);
}


FpsCounter::~FpsCounter()
{
    wMutexDestroy(&mFrameLock);
}


float FpsCounter::getFps()
{
    return mFps;
}


void FpsCounter::updateOnFrame()
{
    wMutexLock(&mFrameLock);

    wTime nowTime;
    wTimeNow(&nowTime);
    mFrameTimeQ.push(nowTime);
    unsigned long periodMs = (mFrameTimeQ.size() > 1) ?
        wTimeDiffMs(&mFrameTimeQ.front(), &mFrameTimeQ.back()) : MS_IN_SEC;

    // Remove all frames that are out of 1ms period
    while (mFrameTimeQ.size() > 1 &&
           periodMs > MS_IN_SEC)
    {
        mFrameTimeQ.pop();
        periodMs = wTimeDiffMs(&mFrameTimeQ.front(), &mFrameTimeQ.back());
    }

    mFps = ((mFrameTimeQ.size() - 1) * MS_IN_SEC) / static_cast<float>(periodMs);

    wMutexUnlock(&mFrameLock);
}
