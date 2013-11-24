/*
 * log.h
 */

#include <stdio.h>
#include <stdbool.h>

#ifndef LOG_H_
#define LOG_H_

// Logging level:
// 0 Logging off
// 1 Error
// 2 Error & Debug
// 3 Error & Debug & Verbose
// 4 Error & Debug & Verbose & Function-Enter-Exit
#ifndef LOGGING_LEVEL
#define LOGGING_LEVEL 4
#endif

int copy_log_to_file_init_by_name(const char* file_name);
int copy_log_to_file_init_by_descriptor(FILE* file_descr);
int copy_log_to_file_deinit();

int internal_init(FILE* file_descr, const char* file_name);

void print_usr_log(char type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...);
void print_enter_exit_func(const char* file, int line, const char* func, bool enter_func);

#if LOGGING_LEVEL > 0
#define LOGE(tag, usrfmt, ...) print_usr_log('E', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGE(tag, usrfmt, ...)
#endif

#if LOGGING_LEVEL > 1
#define LOGD(tag, usrfmt, ...) print_usr_log('D', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGD(tag, usrfmt, ...)
#endif

#if LOGGING_LEVEL > 2
#define LOGV(tag, usrfmt, ...) print_usr_log('V', tag, __FILE__, __LINE__, __FUNCTION__, usrfmt, ##__VA_ARGS__)
#else
#define LOGV(tag, usrfmt, ...)
#endif

#if LOGGING_LEVEL > 3
#define LOG_ENTER_FUNC print_enter_exit_func(__FILE__, __LINE__, __FUNCTION__, true)
#define LOG_EXIT_FUNC  print_enter_exit_func(__FILE__, __LINE__, __FUNCTION__, false)
#else
#define LOG_ENTER_FUNC
#define LOG_EXIT_FUNC
#endif

#endif /* LOG_H_ */
