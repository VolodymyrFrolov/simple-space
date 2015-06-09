//
//  logs.c
//
//  Created by Vladimir Frolov
//

#include "logs.h"

#define NSEC_IN_MSEC 1000000

#if defined (__APPLE__) || defined(__linux__)
#define getThreadId()    pthread_self()
typedef pthread_mutex_t  lock_t;
#elif defined(__WIN32__)
#define getThreadId()    GetCurrentThreadId()
typedef CRITICAL_SECTION lock_t;
#endif

static lock_t       gLogLock;
static bool         gLogInitDone = false;
#if defined(__APPLE__)
static clock_serv_t gClockServ;
#endif

// wTimeInit() is executed inside
void logsInit()
{
    assert(!gLogInitDone);
    if (!gLogInitDone)
    {
        gLogInitDone = true;

        int ret;

        #if defined(__APPLE__)
        ret = host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &gClockServ);
        assert(ret == 0);
        #endif

        #if defined(__APPLE__) || defined(__linux__)
        ret = pthread_mutex_init(&gLogLock, NULL);
        assert(ret == 0);
        #elif defined(__WIN32__)
        InitializeCriticalSection(&gLogLock);
        (void)ret; // Unused
        #endif
    }
}

void logsDeinit()
{
    assert(gLogInitDone);
    if (gLogInitDone)
    {
        gLogInitDone = false;
        
        int ret;

        #if defined(__APPLE__)
        ret = mach_port_deallocate(mach_task_self(), gClockServ);
        assert(ret == 0);
        #endif

        #if defined(__APPLE__) || defined(__linux__)
        ret = pthread_mutex_destroy(&gLogLock);
        assert(ret == 0);
        #elif defined(__WIN32__)
        DeleteCriticalSection(&gLogLock);
        (void)ret; // Unused
        #endif
    }
}

unsigned long getCurrentMilliseconds()
{
    #if defined(__APPLE__) 
    
    mach_timespec_t mts;
    int ret = clock_get_time(gClockServ, &mts);
    assert(ret == 0);
    return mts.tv_nsec / NSEC_IN_MSEC;
    
    #elif defined(__linux__)
    
    struct timespec ts;
    int ret = clock_gettime(CLOCK_REALTIME, &ts);
    assert(ret == 0);
    return ts.tv_nsec / NSEC_IN_MSEC;

    #elif defined(__WIN32__)

    SYSTEMTIME st;
    GetSystemTime(&st);
    return st.wMilliseconds;

    #endif
}

void logWrite(char logType, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...)
{
    int ret;
    char timestamp[64]; // yyyy-mm-dd hh:mm:ss
    char prefix[256];   // date/time threadId logType file line tag function
    char usrmsg[256];   // usrfmt + args

    #if defined(__APPLE__) || defined(__linux__)
    ret = pthread_mutex_lock(&gLogLock);
    assert(ret == 0);
    #elif defined(__WIN32__)
    EnterCriticalSection(&gLogLock);
    #endif

    va_list args;
   
    // Timestamp
    time_t now = time(NULL);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_struct);

    unsigned long threadId = (unsigned long)getThreadId();
    unsigned long milliseconds = getCurrentMilliseconds();

    // Stamp (full, including timestamp)
    ret = snprintf(prefix, sizeof(prefix), "%s.%03lu %s %c %lu %s:%-4d %s: ",
            timestamp, milliseconds, tag, logType, threadId, file, line, func);
    assert(ret > 0);

    // User message from usrfmt and args
    va_start(args, usrfmt);
    ret = vsnprintf(usrmsg, sizeof(usrmsg), usrfmt, args);
    assert(ret > 0);

    // Final print to stdout
    ret = printf("%s%s\n", prefix, usrmsg);
    assert(ret > 0);

    va_end(args);

    #if defined(__APPLE__) || defined(__linux__)
    ret = pthread_mutex_unlock(&gLogLock);
    assert(ret == 0);
    #elif defined(__WIN32__)
    LeaveCriticalSection(&gLogLock);
    #endif
}
