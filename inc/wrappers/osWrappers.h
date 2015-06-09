//
//  osWrappers.h
//
//  Created by Vladimir Frolov
//

#ifndef _OS_WRAPPERS_H_
#define _OS_WRAPPERS_H_

#include <stdlib.h>  // malloc(), free()
#include <stdint.h>  // uintptr_t
#include <stdbool.h> // bool
#include <assert.h>  // assert() defining NDEBUG will disable asserts in code

#if defined(__APPLE__) || defined(__linux__)

    #include <dlfcn.h>   // dlopen(), dlclose(), dlsym()
    #include <time.h>    // nanosleep() clock_gettime()
    #include <pthread.h> // pthread
    #include <errno.h>   // ETIMEDOUT

    #if defined(__APPLE__)
    #include <mach/clock.h> // clock_get_time()
    #include <mach/mach.h>  // host_get_clock_service()
    #endif

    typedef struct timespec   wTime;
    typedef void*             wLib;
    typedef pthread_mutex_t   wMutex;
    typedef pthread_t         wThread;
    typedef struct {
        pthread_cond_t  cond;
        pthread_mutex_t mutex;
        bool            flag;
    } wEvent;

    #define W_TIMEOUT_INITITE (~(0UL))
    #define W_TIMEOUT_EXPIRED ETIMEDOUT

#elif defined(__WIN32__)

    //#ifndef WIN32_LEAN_AND_MEAN
    //#define WIN32_LEAN_AND_MEAN
    //#endif
    #include <windows.h>

    typedef LARGE_INTEGER     wTime;
    typedef HMODULE           wLib;
    typedef CRITICAL_SECTION  wMutex;
    typedef HANDLE            wThread;
    typedef HANDLE            wEvent;

    #define W_TIMEOUT_INITITE INFINITE
    #define W_TIMEOUT_EXPIRED WAIT_TIMEOUT

#else

    #error "Unsupported platform"

#endif

// Time

void          wTimeInit();   // MUST be called at the main beginning (for: Mac/Win)
void          wTimeDeinit(); // Optionally may be called at the end  (for: Mac)

void          wTimeNow    (wTime* time);
void          wTimeZero   (wTime* time);
void          wTimeAddMs  (wTime* time, unsigned long millisec);
unsigned long wTimeDiffMs (const wTime* earlier, const wTime* later);

// Using std::this_thread::sleep_for() is better choice
void          wTimeSleepMs(unsigned long millisec);

// Shared Libraries

wLib  wOpenLib(const char* libName);
void  wCloseLib(wLib libHdl);
void* wLoadSym(wLib libHdl, const char* symName);

// Mutexes

void wMutexInit   (wMutex* mutex);
void wMutexDestroy(wMutex* mutex);
void wMutexLock   (wMutex* mutex);
void wMutexUnlock (wMutex* mutex);

// Threads

typedef int (*wThreadFunc)(void* arg);

int  wThreadCreate(wThread* thread, wThreadFunc func, void* arg, bool joinable);
void wThreadJoin(wThread thread, int* p_retval);

// Events

void wEventInit   (wEvent* event);
void wEventDestroy(wEvent* event);
int  wEventWait   (wEvent* event, unsigned long timeoutMs);
void wEventSignal (wEvent* event);
void wEventReset  (wEvent* event);

#endif // _OS_WRAPPERS_H_
