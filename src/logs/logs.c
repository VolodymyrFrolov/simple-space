/*
 * logs.c
 */

#include "logs.h"
#include "wrp_mutex.h"

#if defined(__linux__) || defined (__APPLE__) || defined (__android__)
    #define get_thread_id() pthread_self()
#elif defined(__WIN32__)
    #define get_thread_id() GetCurrentThreadId()
#else
    #error "Unsupported platform"
#endif

static bool logs_init_done = false;
static wrp_mutex_t log_mutex;

// These time valuse are for milliseconds in logs
#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
static struct timeval gTimeForMs;
#elif defined(__WIN32__)
static SYSTEMTIME gTimeForMs;
#endif

void init_logs() {
    if (!logs_init_done) {
        wrp_mutex_init(&log_mutex);
        logs_init_done = true;
    }
}

void deinit_logs() {
    if (logs_init_done) {
        wrp_mutex_destroy(&log_mutex);
        logs_init_done = false;
    }
}

// Used internally only
unsigned long get_current_milliseconds() {

    int milliseconds;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = gettimeofday(&gTimeForMs, NULL);
    assert(ret == 0);
    milliseconds = gTimeForMs.tv_usec/1000;
    #elif defined(__WIN32__)
    GetSystemTime(&gTimeForMs);
    milliseconds = gTimeForMs.wMilliseconds;
    #endif

    return milliseconds;
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
        case LOG_LEVEL_INFO:
            type_ch = 'I';
            break;
        case LOG_LEVEL_DEBUG:
            type_ch = 'D';
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
    printf("%s%s\n", stamp, usrmes);

    va_end (args);

    wrp_mutex_unlock(&log_mutex);
}
