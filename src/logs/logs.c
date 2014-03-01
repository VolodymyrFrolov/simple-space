/*
 * logs.c
 */

#include <stdarg.h> // var args
#include <time.h>
#include "logs.h"

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #define get_thread_id() pthread_self()
#elif defined(__WIN32__)
    #define get_thread_id() GetCurrentThreadId()
#else
    #error "Unsupported platform"
#endif

static FILE* p_log_file = NULL;
static wrp_mutex_t log_mutex;
bool logs_init_done = false;

int init_logs() {
    int ret = wrp_mutex_init(&log_mutex);
    if (ret != 0) {
        printf("init_logs: Error: pthread_mutex_init ret:[%d]\n", ret);
    } else {
        logs_init_done = true;
    }
    return ret;
}

int deinit_logs() {
    int ret = wrp_mutex_destroy(&log_mutex);
    if (ret != 0) {
        printf("init_logs: Error: pthread_mutex_destroy ret:[%d]\n", ret);
    } else {
        logs_init_done = false;
    }
    return ret;
}

int init_write_logs_to_file(const char* file_name) {
    if (!logs_init_done || p_log_file != NULL) {
        printf("init_write_logs_to_file: Error: logs are not inited or log file is already opened\n");
        return -1;
    }

    p_log_file = fopen(file_name, "w");

    if (p_log_file == NULL) {
        printf("init_write_logs_to_file: Error: log file could not be created/opened\n");
        return -1;
    }

    return 0;
}

int deinit_write_logs_to_file() {
    if (p_log_file == NULL) {
        printf("deinit_write_logs_to_file: Error: log file is already closed\n");
        return -1;
    }
    fclose(p_log_file);
    p_log_file = NULL;
    return 0;
}

void print_usr_log(int type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...)
{
    char stamp[200];  // date-time thread_id log-type file line tag function
    char usrmes[300]; // usrfmt + args
    char timestamp[50];
    char type_ch;

    switch (type)
    {
        case LOG_LEVEL_ERROR:
            type_ch = 'E';
            break;
        case LOG_LEVEL_DEBUG:
            type_ch = 'D';
            break;
        case LOG_LEVEL_VERBOSE:
            type_ch = 'V';
            break;
        default:
            type_ch = '0'; // Should not get here
            break;
    }

    if (logs_init_done) {
        if (wrp_mutex_lock(&log_mutex) != 0)
            printf("print_usr_log: Error locking mutex\n");
    }

    va_list args;
   
    // Timestamp
    time_t now = time(0);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_struct);

    unsigned long thread_id = (unsigned long)get_thread_id();

    // Stamp (full, including timestamp)
    snprintf(stamp, sizeof(stamp), "%s %lu %c [%s] %s:%-3d %s: ", timestamp, thread_id, type_ch, tag, file, line, func);

    // User message from usrfmt and args
    va_start(args, usrfmt);
    vsnprintf(usrmes, sizeof(usrmes), usrfmt, args);

    // Final print
    //printf("%s%s\n", stamp, usrmes);

    // Copy log to file
    if (p_log_file != NULL) {
        fprintf(p_log_file, "%s%s\n", stamp, usrmes);
        fflush(p_log_file);
    }

    va_end (args);

    if (logs_init_done) {
        if (wrp_mutex_unlock(&log_mutex) != 0)
            printf("print_usr_log: Error unlocking mutex\n");
    }
}

void print_enter_exit_func(const char* tag, const char* file, int line, const char* func, bool enter_func)
{
    // Timestamp
    char timestamp[50];
    char full_msg[300];
    char type_ch;
    const char* type_str;

    if (logs_init_done) {
        if (wrp_mutex_lock(&log_mutex) != 0)
            printf("print_enter_exit_func: Error locking mutex\n");
    }

    time_t now = time(0);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_struct);

    // Enter/Exit label (+/-)
    if (enter_func) {
        type_ch = '+';
        type_str = "ENTER";
    } else {
        type_ch = '-';
        type_str = "EXIT";
    }

    unsigned long thread_id = (unsigned long)get_thread_id();

    snprintf(full_msg, sizeof(full_msg), "%s %lu %c [%s] %s:%-3d %s: %s", timestamp, thread_id, type_ch, tag, file, line, func, type_str);

    // Final print    
    //printf("%s\n", full_msg);

    // Copy log to file
    if (p_log_file != NULL) {
        fprintf(p_log_file, "%s\n", full_msg);
        fflush(p_log_file);
    }
    
    if (logs_init_done) {
        if (wrp_mutex_unlock(&log_mutex) != 0)
            printf("print_enter_exit_func: Error unlocking mutex\n");
    }
}
