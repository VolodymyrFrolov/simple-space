/*
 * log.c
 */

#include <stdarg.h> // var args
#include <time.h>
#include "log.h"
#include "mutex_wrapper.h"

#if defined(__linux__) || defined(__android__) || defined(__APPLE__)
    #include <unistd.h> // syscall()
    #include <sys/syscall.h> // gettid()
    #define get_current_thread_id() syscall(SYS_gettid)

#elif defined (__WIN32__)
    #include <windows.h>
    #define get_current_thread_id() GetCurrentThreadId()
#else
    #error "Unsupported platform"
#endif

static bool copy_to_file_enabled = false;
static FILE* f_descr = NULL;
static mutex_hdl m_hdl;

int copy_log_to_file_init_by_name(const char* file_name) {
    // Warning: Comparison between string literal results unspecified behaviour
    //if (file_name == NULL || file_name == "")
    //    return -1;
    //else
        return internal_init(NULL, file_name);
}

int copy_log_to_file_init_by_descriptor(FILE* file_descr) {
    if (file_descr == NULL)
        return -1;
    else
        return internal_init(file_descr, NULL);
}

int internal_init(FILE* file_descr, const char* file_name) {
    int ret = 0;

    if (!copy_to_file_enabled) {
        if (file_descr == NULL) {
            f_descr = fopen(file_name, "w");
            if (f_descr == NULL)
                return -1;
        } else {
            f_descr = file_descr;
        }
        
        ret = mutex_init_wrapper(&m_hdl);
        if (ret != 0)
            return -1;

        copy_to_file_enabled = true;
    }

    return ret;
}

int copy_log_to_file_deinit() {
    int ret = 0;

    if (copy_to_file_enabled) {
        fclose(f_descr);

        ret = mutex_deinit_wrapper(&m_hdl);
        if (ret != 0)
            return -1;

        copy_to_file_enabled = false;
    }
    return ret;
}

void print_usr_log(char type, const char* tag, const char* file, int line, const char* func, const char* usrfmt, ...)
{
    char stamp[200];  // date-time thread_id log-type file line tag function
    char usrmes[300]; // usrfmt + args
    char timestamp[50];
    va_list args;

    if (copy_to_file_enabled)
        mutex_lock_wrapper(&m_hdl);
   
    // Timestamp
    time_t now = time(0);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_struct);

    // Thread id
    unsigned long thread_id = (unsigned long)get_current_thread_id();

    // Stamp (full, including timestamp)
    snprintf(stamp, sizeof(stamp), "%s %lu %c [%s] %s:%-3d %s: ", timestamp, thread_id, type, tag, file, line, func);

    // User message from usrfmt and args
    va_start(args, usrfmt);
    vsnprintf(usrmes, sizeof(usrmes), usrfmt, args);

    // Final print
    printf("%s%s\n", stamp, usrmes);

    // Also copy log to file
    if (copy_to_file_enabled && (f_descr != NULL)) {
        fprintf(f_descr, "%s%s\n", stamp, usrmes);
        fflush(f_descr);
    }

    if (copy_to_file_enabled)
        mutex_unlock_wrapper(&m_hdl);

    va_end (args);
}

void print_enter_exit_func(const char* file, int line, const char* func, bool enter_func)
{
    // Timestamp
    char timestamp[50];

    if (copy_to_file_enabled)
        mutex_lock_wrapper(&m_hdl);

    time_t now = time(0);
    struct tm* tm_struct = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%d/%m/%Y %H:%M:%S", tm_struct);

    // Thread id
    unsigned long thread_id = (unsigned long)get_current_thread_id();

    // Enter/Exit label (+/-)
    char status_ch;
    const char* status_str;
    if (enter_func) {
        status_ch = '+';
        status_str = "ENTER";
    } else {
        status_ch = '-';
        status_str = "EXIT";
    }

    // Final print
    printf("%s %lu %c %s:%-3d %s: %s\n", timestamp, thread_id, status_ch, file, line, func, status_str);

    // Also copy log to file
    if (copy_to_file_enabled && (f_descr != NULL)) {
        fprintf(f_descr, "%s %lu %c %s:%-3d %s: %s\n", timestamp, thread_id, status_ch, file, line, func, status_str);
        fflush(f_descr);
    }
    
    if (copy_to_file_enabled)
        mutex_unlock_wrapper(&m_hdl);
}
