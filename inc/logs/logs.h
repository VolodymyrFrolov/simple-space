
/*
 * logs.h
 */

#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h> // var args
#include <time.h>
#include <assert.h> // #define NDEBUG will disable asserts in code

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #include <sys/time.h> // gettimeofday()
    #include <pthread.h>  // thread id
#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>  // thread id
#else
    #error "Unsupported platform"
#endif

// Use LOG_LEVEL to configure logging level
// Use INIT_LOGS()/DEINIT_LOGS() before/after logging

// Options
#define LOG_LEVEL_OFF   0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3

// Defaults
#ifndef LOG_LEVEL
#define LOG_LEVEL      LOG_LEVEL_INFO
#endif

// LOG_LEVEL
void print_usr_log(int type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...);

#if   (LOG_LEVEL == LOG_LEVEL_OFF)
#define LOGE(tag, usrfmt, ...)
#define LOGI(tag, usrfmt, ...)
#define LOGD(tag, usrfmt, ...)

#elif (LOG_LEVEL == LOG_LEVEL_ERROR)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGI(tag, usrfmt, ...)
#define LOGD(tag, usrfmt, ...)

#elif (LOG_LEVEL == LOG_LEVEL_INFO)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGI(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_INFO,  tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGD(tag, usrfmt, ...)

#elif (LOG_LEVEL == LOG_LEVEL_DEBUG)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGI(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_INFO,  tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGD(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_DEBUG, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)

#else
#error "Invalid LOG_LEVEL option"
#endif

// INIT / DEINIT
void init_logs();
void deinit_logs();

#define INIT_LOGS()   init_logs()
#define DEINIT_LOGS() deinit_logs()

#endif /* LOGS_H_ */
