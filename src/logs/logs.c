/*
 * logs.c
 */

#include "logs.h"

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #define get_thread_id() pthread_self()
#elif defined(__WIN32__)
    #define get_thread_id() GetCurrentThreadId()
#else
    #error "Unsupported platform"
#endif

static bool logs_init_done = false;
static FILE* p_log_file = NULL;
static wrp_mutex_t log_mutex;

// These time valuse are for milliseconds in logs
#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
static struct timeval gTimeForMs;
#elif defined(__WIN32__)
static SYSTEMTIME gTimeForMs;
#endif

void init_logs_wothout_file() {
    if (!logs_init_done) {
        wrp_mutex_init(&log_mutex);
        logs_init_done = true;
    }
}

void init_logs_with_file(const char* filename) {
    if (!logs_init_done) {
        wrp_mutex_init(&log_mutex);

        if ((filename == NULL) || (filename[0] == '\0')) {
            assert(0); // Error case
        }

        p_log_file = fopen(filename, "w");
        if (p_log_file == NULL) {
            assert(0); // Error case
        }

        logs_init_done = true;
    }
}

void deinit_logs_func() {
    if (logs_init_done) {
        wrp_mutex_destroy(&log_mutex);

        if ((LOG_DEST == LOG_DEST_FILE) || (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)) {
            if (p_log_file == NULL) {
                assert(0); // Error case
            }

            int ret = fclose(p_log_file);
            assert(ret == 0);
            p_log_file = NULL;
        }

        logs_init_done = false;
    }
}

// Used internally only
unsigned long get_current_milliseconds() {

    int retval;
    int retcode;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    retcode = gettimeofday(&gTimeForMs, NULL);
    assert(retcode == 0);
    retval = gTimeForMs.tv_usec/1000;
    #elif defined(__WIN32__)
    GetSystemTime(&gTimeForMs);
    retval = gTimeForMs.wMilliseconds;
    #endif

    return retval;
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

    wrp_mutex_lock(&log_mutex);

    va_list args;
   
    // Timestamp
    time_t now = time(0);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_struct);

    unsigned long thread_id = (unsigned long)get_thread_id();
    unsigned long ms = get_current_milliseconds();

    // Stamp (full, including timestamp)
    snprintf(stamp, sizeof(stamp), "%s.%lu %lu %c [%s] %s:%-3d %s: ", timestamp, ms, thread_id, type_ch, tag, file, line, func);

    // User message from usrfmt and args
    va_start(args, usrfmt);
    vsnprintf(usrmes, sizeof(usrmes), usrfmt, args);

    // Final print to stdout
    if ((LOG_DEST == LOG_DEST_STDOUT) || (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)) {
        printf("%s%s\n", stamp, usrmes);
    }

    // Final print to file
    if ((LOG_DEST == LOG_DEST_FILE) || (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)) {
        fprintf(p_log_file, "%s%s\n", stamp, usrmes);
        fflush(p_log_file);
    }

    va_end (args);

    wrp_mutex_unlock(&log_mutex);
}

void print_enter_exit_func(const char* tag, const char* file, int line, const char* func, bool enter_func)
{
    // Timestamp
    char timestamp[50];
    char full_msg[300];
    char type_ch;
    const char* type_str;

    wrp_mutex_lock(&log_mutex);

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
    unsigned long ms = get_current_milliseconds();

    snprintf(full_msg, sizeof(full_msg), "%s.%lu %lu %c [%s] %s:%-3d %s: %s", timestamp, ms, thread_id, type_ch, tag, file, line, func, type_str);

    // Final print to stdout
    if ((LOG_DEST == LOG_DEST_STDOUT) || (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)) {
        printf("%s\n", full_msg);
    }

    // Final print to file
    if ((LOG_DEST == LOG_DEST_FILE) || (LOG_DEST == LOG_DEST_STDOUT_AND_FILE)) {
        fprintf(p_log_file, "%s\n", full_msg);
        fflush(p_log_file);
    }
    
    wrp_mutex_unlock(&log_mutex);
}
