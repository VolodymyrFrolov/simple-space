/*
 * logs.h
 */

#ifndef LOGS_H_
#define LOGS_H_

#include <stdio.h>
#include <stdbool.h>
#include "wrp_mutex.h"

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #include <pthread.h>
#elif defined(__WIN32__)
    #include <windows.h>
#else
    #error "Unsupported platform"
#endif

#ifndef LOG_LEVEL
#define LOG_LEVEL 2
#endif

#ifndef LOG_ENTER_EXIT
#define LOG_ENTER_EXIT 0
#endif

#define LOG_LEVEL_DISABLED 0
#define LOG_LEVEL_ERROR    1
#define LOG_LEVEL_DEBUG    2
#define LOG_LEVEL_VERBOSE  3

int init_logs();
int deinit_logs();

int init_write_logs_to_file(const char* file_name);
int deinit_write_logs_to_file();

void print_usr_log(int type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...);
void print_enter_exit_func(const char* tag, const char* file, int line, const char* func, bool enter_func);

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOGE(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGE(tag, usrfmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOGD(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_DEBUG, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGD(tag, usrfmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOGV(tag, usrfmt, ...) print_usr_log(LOG_LEVEL_VERBOSE, tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGV(tag, usrfmt, ...)
#endif

#if LOG_ENTER_EXIT > 0
#define LOG_ENTER_FUNC(tag) print_enter_exit_func(tag, __FILE__, __LINE__, __FUNCTION__, true)
#define LOG_EXIT_FUNC(tag)  print_enter_exit_func(tag, __FILE__, __LINE__, __FUNCTION__, false)
#else
#define LOG_ENTER_FUNC(tag)
#define LOG_EXIT_FUNC(tag)
#endif

#endif /* LOGS_H_ */
