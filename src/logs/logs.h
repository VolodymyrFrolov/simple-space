/*
 * logs.h
 */

#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>
#include <stdbool.h>
#include <assert.h> // #define NDEBUG will disable asserts in code
#include "wrp_mutex.h"

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #include <pthread.h> // for thread id
#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h> // for thread id
#else
    #error "Unsupported platform"
#endif

// Use LOG_DEST, LOG_LEVEL, LOG_ENTER_EXIT to configure logging
// Use INIT_LOGS

// Options
#define LOG_LEVEL_DISABLED 0
#define LOG_LEVEL_ERROR    1
#define LOG_LEVEL_DEBUG    2
#define LOG_LEVEL_VERBOSE  3

#define LOG_ENTER_EXIT_OFF 0
#define LOG_ENTER_EXIT_ON  1 // Does not work if LOG_LEVEL_DISABLED

#define LOG_DEST_STDOUT          1
#define LOG_DEST_FILE            2
#define LOG_DEST_STDOUT_AND_FILE 3

// Defaults
#ifndef LOG_LEVEL
#define LOG_LEVEL      LOG_LEVEL_DEBUG
#endif

#ifndef LOG_ENTER_EXIT
#define LOG_ENTER_EXIT LOG_ENTER_EXIT_OFF
#endif

#ifndef LOG_DEST
#define LOG_DEST       LOG_DEST_STDOUT_AND_FILE
#endif

// LOG_LEVEL
void print_usr_log(int type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...);
void print_enter_exit_func(const char* tag, const char* file, int line, const char* func, bool enter_func);

#if   (LOG_LEVEL == LOG_LEVEL_DISABLED)
#define LOGE(tag, usrfmt, ...)
#define LOGD(tag, usrfmt, ...)
#define LOGV(tag, usrfmt, ...)
#undef  LOG_ENTER_EXIT
#define LOG_ENTER_EXIT LOG_ENTER_EXIT_OFF // Disable Enter/Exit logs

#elif (LOG_LEVEL == LOG_LEVEL_ERROR)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR,   tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGD(tag, usrfmt, ...)
#define LOGV(tag, usrfmt, ...)

#elif (LOG_LEVEL == LOG_LEVEL_DEBUG)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR,   tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGD(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_DEBUG,   tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGV(tag, usrfmt, ...)

#elif (LOG_LEVEL == LOG_LEVEL_VERBOSE)
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR,   tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGD(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_DEBUG,   tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#define LOGV(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_VERBOSE, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)

#else
#error "Invalid LOG_LEVEL option"
#endif

// LOG_ENTER_EXIT
#if   (LOG_ENTER_EXIT == LOG_ENTER_EXIT_OFF)
#define LOG_ENTER_FUNC(tag)
#define LOG_EXIT_FUNC(tag)

#elif (LOG_ENTER_EXIT == LOG_ENTER_EXIT_ON)
#define LOG_ENTER_FUNC(tag) print_enter_exit_func(tag, __FILE__, __LINE__, __FUNCTION__, true)
#define LOG_EXIT_FUNC(tag)  print_enter_exit_func(tag, __FILE__, __LINE__, __FUNCTION__, false)

#else
#error "Invalid LOG_ENTER_EXIT option"
#endif

// INIT / DEINIT
void init_logs_wothout_file();
void init_logs_with_file(const char* filename);
void deinit_logs_func();

#if   (LOG_DEST == LOG_DEST_STDOUT)
#define INIT_LOGS()         init_logs_wothout_file()
#define INIT_LOGS(filename) init_logs_wothout_file()

#elif (LOG_DEST == LOG_DEST_FILE)
#define INIT_LOGS(filename) init_logs_with_file(filename)

#elif (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)
#define INIT_LOGS(filename) init_logs_with_file(filename)

#else
#error "Invalid LOG_DEST option"
#endif

#define DEINIT_LOGS() deinit_logs_func()

#endif /* LOGS_H_ */
