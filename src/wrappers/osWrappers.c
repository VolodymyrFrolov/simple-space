//
//  osWrappers.c
//
//  Created by Vladimir Frolov
//

#include "osWrappers.h"

// Time

#define NUM_1e3 1000
#define NUM_1e6 1000000
#define NUM_1e9 1000000000

static bool gTimeInitDone = false;
#if defined(__APPLE__)
static clock_serv_t gClockServ;
#elif defined(__WIN32__)
static LARGE_INTEGER gFrequency;
#endif

void wTimeInit()
{
    assert(!gTimeInitDone);
    if (!gTimeInitDone)
    {
        #if defined(__APPLE__)
        int ret = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &gClockServ);
        assert(ret == 0);
        #elif defined(__WIN32__)
        int ret = QueryPerformanceFrequency(&gFrequency);
        assert(ret != 0);
        #endif
        gTimeInitDone = true;
    }
}

void wTimeDeinit()
{
    assert(gTimeInitDone);
    if (gTimeInitDone)
    {
        #if defined(__APPLE__)
        int ret = mach_port_deallocate(mach_task_self(), gClockServ);
        assert(ret == 0);
        #endif
        gTimeInitDone = false;
    }
}

void wTimeNow(wTime* time)
{
    #if defined(__APPLE__)

    // Mac OS does not have clock_gettime, use clock_get_time
    // Another option is to use gettimeofday() for both Linux and Mac
    // but it's deprecated and not recommended due to not being monotonic

    mach_timespec_t mts;
    int ret = clock_get_time(gClockServ, &mts);
    assert(ret == 0);

    time->tv_sec  = mts.tv_sec;
    time->tv_nsec = mts.tv_nsec;

    #elif defined(__linux__)

    int ret = clock_gettime(CLOCK_REALTIME, time);
    assert(ret == 0);

    #elif defined(__WIN32__)

    int ret = QueryPerformanceCounter(time);
    assert(ret != 0);

    #endif
}

void wTimeZero(wTime* time)
{
    #if defined(__APPLE__) || defined(__linux__)
    time->tv_sec  = 0;
    time->tv_nsec = 0;
    #elif defined(__WIN32__)
    time->QuadPart = 0;
    #endif
}

void wTimeAddMs(wTime* time, unsigned long millisec)
{
    #if defined(__APPLE__) || defined(__linux__)
    time->tv_sec  += (millisec / NUM_1e3);
    time->tv_nsec += (millisec % NUM_1e3) * NUM_1e6;

    if (time->tv_nsec >= NUM_1e9) {
        time->tv_sec++;
        time->tv_nsec -= NUM_1e9;
    }
    #elif defined(__WIN32__)
    time->QuadPart += (gFrequency.QuadPart * millisec) / NUM_1e3;
    #endif
}

unsigned long wTimeDiffMs(const wTime* earlier, const wTime* later)
{
    #if defined(__APPLE__) || defined(__linux__)
    assert(later->tv_sec >= earlier->tv_sec);
    return (later->tv_sec - earlier->tv_sec) * NUM_1e3 + (later->tv_nsec - earlier->tv_nsec) / NUM_1e6;
    #elif defined(__WIN32__)
    assert(later->QuadPart >= earlier->QuadPart);
    // To get usec, change NUM_1e3 to NUM_1e6
    return ((later->QuadPart - earlier->QuadPart) * NUM_1e3) / gFrequency.QuadPart;
    #endif
}

// Using std::this_thread::sleep_for() is better choice
void wTimeSleepMs(unsigned long millisec)
{
    #if defined(__APPLE__) || defined(__linux__)
    struct timespec req;
    req.tv_sec = millisec / NUM_1e3;
    req.tv_nsec = (millisec % NUM_1e3) * NUM_1e6;
    int ret = nanosleep(&req, NULL);
    assert(ret == 0);
    #elif defined(__WIN32__)
    Sleep(millisec);
    #endif
}

// Shared Libraries

wLib wOpenLib(const char* libName)
{
    #if defined(__APPLE__) || defined(__linux__)
    return dlopen(libName, RTLD_NOW);
    #elif defined(__WIN32__)
    return LoadLibrary(libName);
    #endif
}

void wCloseLib(wLib lib)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret = dlclose(lib);
    assert(ret == 0);

    #elif defined(__WIN32__)
    int ret = FreeLibrary(lib);
    assert(ret != 0);
    #endif
}

void* wLoadSym(wLib lib, const char* symName)
{
    #if defined(__APPLE__) || defined(__linux__)
    return dlsym(lib, symName);
    #elif defined(__WIN32__)
    return GetProcAddress(lib, symName);
    #endif
}

// Mutexes

void wMutexInit(wMutex* mutex)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret = pthread_mutex_init(mutex, NULL);
    assert(ret == 0);

    #elif defined(__WIN32__)
    InitializeCriticalSection(mutex);
    #endif
}

void wMutexDestroy(wMutex* mutex)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret = pthread_mutex_destroy(mutex);
    assert(ret == 0);

    #elif defined(__WIN32__)
    DeleteCriticalSection(mutex);
    #endif
}

void wMutexLock(wMutex* mutex)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret = pthread_mutex_lock(mutex);
    assert(ret == 0);
    
    #elif defined(__WIN32__)
    EnterCriticalSection(mutex);
    #endif
}

void wMutexUnlock(wMutex* mutex)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret = pthread_mutex_unlock(mutex);
    assert(ret == 0);
    
    #elif defined(__WIN32__)
    LeaveCriticalSection(mutex);
    #endif
}

// Threads

typedef struct {
    wThreadFunc func;
    void*       arg;
} wFuncAndArg;

#if defined(__APPLE__) || defined(__linux__)
static void*        staticEntryPoint(void* arg)
#elif defined(__WIN32__)
static DWORD WINAPI staticEntryPoint(void* arg)
#endif
{
    wFuncAndArg* pFuncAndArg = (wFuncAndArg*)arg;
    int ret = pFuncAndArg->func(pFuncAndArg->arg);
    free(pFuncAndArg);

    #if defined(__APPLE__) || defined(__linux__)
    return (void*)(uintptr_t)ret; // Also pthread_exit((void*)ret) may be used
    #elif defined(__WIN32__)
    return (DWORD)ret;
    #endif
}

int wThreadCreate(wThread* thread, wThreadFunc func, void* arg, bool joinable)
{
    wFuncAndArg* pFuncAndArg = (wFuncAndArg*)malloc(sizeof(wFuncAndArg));
    assert(pFuncAndArg != NULL);
    if (pFuncAndArg == NULL) {
        return -1;
    }
    pFuncAndArg->func = func;
    pFuncAndArg->arg = arg;

    #if defined(__APPLE__) || defined(__linux__)

    pthread_attr_t attr;
    int ret = pthread_attr_init(&attr);
    assert(ret == 0);

    ret = (joinable) ?
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) :
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(ret == 0);

    ret = pthread_create(thread, &attr, staticEntryPoint, pFuncAndArg);
    assert(ret == 0);
    if (ret != 0) {
        return -1;
    }

    ret = pthread_attr_destroy(&attr);
    assert(ret == 0);

    #elif defined(__WIN32__)

    *thread = CreateThread(NULL, 0, staticEntryPoint, pFuncAndArg, 0, NULL);
    assert(*thread != NULL);
    if (*thread == NULL) {
        return -1;
    }

    if (!joinable) {
        int ret = CloseHandle(thread);
        assert(close_ret != 0);
    }

    #endif

    return 0;
}

void wThreadJoin(wThread thread, int* p_retval)
{
    #if defined(__APPLE__) || defined(__linux__)
    
    int ret;
    if (p_retval != NULL) {
        void* thread_ret;
        ret = pthread_join(thread, &thread_ret);
        *p_retval = (int)(uintptr_t)thread_ret;
    } else {
        ret = pthread_join(thread, NULL);
    }
    assert(ret == 0);

    #elif defined(__WIN32__)

    int ret = WaitForSingleObject(thread, INFINITE);
    assert(ret == WAIT_OBJECT_0);

    if (p_retval != NULL) {
        DWORD thread_ret;
        ret = GetExitCodeThread(thread, &thread_ret);
        assert(ret != 0);
        *p_retval = (int)thread_ret;
    }

    ret = CloseHandle(thread);
    assert(ret != 0);

    #endif
}

// Events

void wEventInit(wEvent* event)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret;
    event->flag = false;
    ret = pthread_mutex_init(&event->mutex, NULL);
    assert(ret == 0);
    ret = pthread_cond_init(&event->cond, NULL);
    assert(ret == 0);

    #elif defined(__WIN32__)
    *event = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(*event != NULL);
    #endif
}

void wEventDestroy(wEvent* event)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret;
    ret = pthread_mutex_destroy(&event->mutex);
    assert(ret == 0);
    ret = pthread_cond_destroy(&event->cond);
    assert(ret == 0);

    #elif defined(__WIN32__)
    int ret = CloseHandle(*event);
    assert(ret != 0);
    #endif
}

int wEventWait(wEvent* event, unsigned long timeoutMs)
{
    #if defined(__APPLE__) || defined(__linux__)

    int ret;
    int wait_ret;

    ret = pthread_mutex_lock(&event->mutex);
    assert(ret == 0);

    if (timeoutMs == W_TIMEOUT_INITITE)
    {
        while (!event->flag)
        {
            wait_ret = pthread_cond_wait(&event->cond, &event->mutex);
            assert(wait_ret == 0);
            if (wait_ret != 0) // Unexpected error
            {
                break;
            }
        }
    }
    else
    {
        wTime timeoutAbs;
        wTimeNow(&timeoutAbs);
        wTimeAddMs(&timeoutAbs, timeoutMs);
        while (!event->flag)
        {
            wait_ret = pthread_cond_timedwait(&event->cond, &event->mutex, &timeoutAbs);
            assert(wait_ret == 0 || wait_ret == ETIMEDOUT);
            if (wait_ret != 0) // ETIMEDOUT or Unexpected error
            {
                break;
            }
        }
    }

    // Reset flag if event was signaled
    if (wait_ret == 0) {
        event->flag = false;
    }

    ret = pthread_mutex_unlock(&event->mutex);
    assert(ret == 0);

    return wait_ret;

    #elif defined(__WIN32__)

    int ret = WaitForSingleObject(*event, timeoutMs);
    assert(ret == WAIT_OBJECT_0 || ret == WAIT_TIMEOUT);
    return ret;

    #endif
}

void wEventSignal(wEvent* event)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret;
    ret = pthread_mutex_lock(&event->mutex);
    assert(ret == 0);

    event->flag = true;

    ret = pthread_cond_signal(&event->cond);
    assert(ret == 0);
    
    ret = pthread_mutex_unlock(&event->mutex);
    assert(ret == 0);

    #elif defined(__WIN32__)
    int ret = SetEvent(*event);
    assert(ret != 0);
    #endif
}

void wEventReset(wEvent* event)
{
    #if defined(__APPLE__) || defined(__linux__)
    int ret;
    ret = pthread_mutex_lock(&event->mutex);
    assert(ret == 0);

    event->flag = false;

    ret = pthread_mutex_unlock(&event->mutex);
    assert(ret == 0);

    #elif defined(__WIN32__)
    int ret = ResetEvent(*event);
    assert(ret != 0);
    #endif
}
