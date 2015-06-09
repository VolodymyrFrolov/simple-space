//
//  logs.h
//
//  Created by Vladimir Frolov
//

#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>   // printf
#include <stdbool.h> // bool
#include <stdarg.h>  // va_list, vsnprintf
#include <assert.h>  // assert() defining NDEBUG will disable asserts in code

#if defined(__APPLE__) || defined(__linux__)

    #include <pthread.h>    // pthread_mutex_t, pthread_self()

    #if defined(__APPLE__)
    #include <mach/clock.h> // clock_get_time()
    #include <mach/mach.h>  // host_get_clock_service()
    #else // Linux
    #include <time.h>       // clock_gettime()
    #endif

#elif defined(__WIN32__)

    //#ifndef WIN32_LEAN_AND_MEAN
    //#define WIN32_LEAN_AND_MEAN
    //#endif
    #include <windows.h>    // CRITICAL_SECTION, GetCurrentThreadId()

#else

    #error "Unsupported platform"

#endif

// Use LOGLEVEL to configure logging level at compile time
// Use INIT_LOGS()/DEINIT_LOGS() at program beginning/end

// Init / deinit
// Forward declaration
void logsInit();
void logsDeinit();
void logWrite(char logType, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...);

// Log levels
#define LOGLEVEL_OFF   0
#define LOGLEVEL_ERROR 1
#define LOGLEVEL_INFO  2
#define LOGLEVEL_DEBUG 3

// Default
#ifndef LOGLEVEL
#define LOGLEVEL LOGLEVEL_INFO
#endif

// LOGLEVEL
#if   (LOGLEVEL == LOGLEVEL_OFF)
#define LogE(tag, usrfmt, ...)
#define LogI(tag, usrfmt, ...)
#define LogD(tag, usrfmt, ...)

#elif (LOGLEVEL == LOGLEVEL_ERROR)
#define LogE(tag, usrfmt, ...) logWrite('E', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LogI(tag, usrfmt, ...)
#define LogD(tag, usrfmt, ...)

#elif (LOGLEVEL == LOGLEVEL_INFO)
#define LogE(tag, usrfmt, ...) logWrite('E', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LogI(tag, usrfmt, ...) logWrite('I', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LogD(tag, usrfmt, ...)

#elif (LOGLEVEL == LOGLEVEL_DEBUG)
#define LogE(tag, usrfmt, ...) logWrite('E', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LogI(tag, usrfmt, ...) logWrite('I', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LogD(tag, usrfmt, ...) logWrite('D', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)

#else
#error "Invalid LOGLEVEL option"
#endif



#endif /* LOGS_H_ */
