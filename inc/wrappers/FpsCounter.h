//
//  FpsCounter.h
//
//  Created by Vladimir Frolov
//

#ifndef _FPS_COUNTER_H_
#define _FPS_COUNTER_H_

#include <queue>

extern "C"
{
    #include "osWrappers.h"
}

class FpsCounter
{
    std::queue<wTime> mFrameTimeQ;
    float        mFps;
    wMutex       mFrameLock;

public:
    FpsCounter();
    ~FpsCounter();

    void  updateOnFrame();
    float getFps();
};

#endif /* defined(_FPS_COUNTER_H_) */
