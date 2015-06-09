//
//  Stopwatch.h
//
//  Created by Vladimir Frolov
//

#ifndef _STOPWATCH_H_
#define _STOPWATCH_H_

extern "C"
{
    #include "osWrappers.h"
}

class Stopwatch
{
    bool   mIsRunning;
    wTime  mStartTime;
    wTime  mStopTime;
    wMutex mStateLock;

public:
    Stopwatch(bool isStarted = true);
    ~Stopwatch();

    void          start();
    void          stop();
    unsigned long timeElaplsedMs() const;
    bool          isRunning() const;
};

#endif /* defined(_STOPWATCH_H_) */
