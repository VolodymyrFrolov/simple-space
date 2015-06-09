//
//  test_wrappers.cpp
//
//  Created by Vladimir Frolov
//

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

#include <thread> // std::this_thread::sleep_for
#include <chrono> // std::chrono::seconds

extern "C"
{
    #include "osWrappers.h"
}

#include "Timer.h"
#include "Stopwatch.h"
#include "FpsCounter.h"

#if defined(__APLLE__) || defined(__linux__)
#define SLEEP_MS(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#else
#define SLEEP_MS(ms) wTimeSleepMs(ms)
#endif

wMutex  gMutex;
wEvent  gEvent;
wThread gThread1;
wThread gThread2;

int gProdNumber = 0;

int someFunc(void* arg)
{
    int i;
    int counts = 3;
    for (i = 0; i < counts; ++i)
    {
        wMutexLock(&gMutex);
        printf("thread #%d iteration:%d started\n", (int)(uintptr_t)arg, i + 1);
 
        SLEEP_MS(1000);

        printf("thread #%d iteration:%d finished\n", (int)(uintptr_t)arg, i + 1);
        wMutexUnlock(&gMutex);

        SLEEP_MS(50);
    }

    return (int)(uintptr_t)arg;
}

bool productsAvailable()
{
    return (gProdNumber > 0);
}

int consumerFunc(void* arg)
{
    printf("consuming started\n");
    bool alive = true;
    uint32_t needed = (uintptr_t)arg;

    while (needed > 0)
    {

        while (!productsAvailable())
        {
            int ret = wEventWait(&gEvent, 2000);
            if (ret == 0) {
                printf("consumer waited successfully and ate\n");
            } else if (ret == W_TIMEOUT_EXPIRED) {
                printf("consumer wait timeout\n");
                alive = false;
                break;
            } else {
                printf("consumer wait error\n");
                alive = false;
                break;
            }
        }

        if (alive) {
            wMutexLock(&gMutex);
            --needed;
            --gProdNumber;
            wMutexUnlock(&gMutex);
        }

        if (!alive) {
            printf("consumer died\n");
            break;
        }
        
    }
    printf("consuming finished\n");
    return 0;
}

int producerFunc(void* arg)
{
    printf("producing started\n");
    
    uint32_t needed = (uint32_t)(uintptr_t)arg;
    uint32_t produced = 0;

    while (produced < needed)
    {
        SLEEP_MS(1000);
        
        wMutexLock(&gMutex);
        ++gProdNumber;
        ++produced;
        printf("produced:%d/%d\n", produced, needed);
        wEventSignal(&gEvent);
        
        wMutexUnlock(&gMutex);
    }
    printf("producing finished\n");
    return 0;
}

int onTimerFunc(void* arg)
{
    printf("callback called on timer (arg:%p)\n", arg);
    //SLEEP_MS(200)
    //printf("callback exited (arg:%p)\n", arg);
    return 0;
}

int main(void)
{
    wTimeInit();

    // ==== Test Case 1 ====

    printf("Test Case 1: Started\n");
    wMutexInit(&gMutex);

    wThreadCreate(&gThread1, someFunc, (void*)1, true);
    SLEEP_MS(50);
    wThreadCreate(&gThread2, someFunc, (void*)2, true);

    int ret1;
    int ret2;
    wThreadJoin(gThread1, &ret1);
    wThreadJoin(gThread2, &ret2);

    wMutexDestroy(&gMutex);

    printf("main: thread1 returned: %d\n", ret1);
    printf("main: thread2 returned: %d\n", ret2);
    printf("Test Case 1: Finished\n");

    // ==== Test Case 2 ====

    printf("Test Case 2: Started\n");
    wMutexInit(&gMutex);
    wEventInit(&gEvent);

    wThreadCreate(&gThread1, consumerFunc, (void*)3, true);
    SLEEP_MS(50);
    wThreadCreate(&gThread2, producerFunc, (void*)3, true);

    wThreadJoin(gThread1, NULL);
    wThreadJoin(gThread2, NULL);

    wEventDestroy(&gEvent);
    wMutexDestroy(&gMutex);
    printf("Test Case 2: Finished\n");

    // ==== Test Case 3 ====

    printf("Test Case 3: Started\n");
    Timer timer(onTimerFunc, NULL, 500, true, true);
    SLEEP_MS(3000);
    timer.stop();
    printf("Test Case 3: Finished\n");

    // ==== Test Case 4 ====

    printf("Test Case 4: Started\n");
    Stopwatch stopwatch;
    SLEEP_MS(800);
    printf("stapwatch time elapsed: %lu ms\n", stopwatch.timeElaplsedMs());
    SLEEP_MS(400);
    stopwatch.stop();
    printf("stapwatch time elapsed: %lu ms\n", stopwatch.timeElaplsedMs());
    printf("Test Case 4: Finished\n");

    // ==== Test Case 5 ====

    printf("Test Case 5: Started\n");
    FpsCounter fpsCounter;
    for (int i = 1; i < 201; ++i)
    {
        fpsCounter.updateOnFrame();
        SLEEP_MS(20);
        if (i%50 == 0)
            printf("dummy fps:%f after frame updates:%d\n", fpsCounter.getFps(), i);
    }
    printf("Test Case 5: Finished\n");

    wTimeDeinit();
    return EXIT_SUCCESS;
}
