#ifndef WRP_COND_H_
#define WRP_COND_H_

#include "wrp_mutex.h"

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <pthread.h>
    #include <errno.h>
    #include <sys/time.h> // clock_gettime from <linux/time.h> is not available for mac
    #define wrp_cond_t pthread_cond_t
    #define WRP_TIMEOUT_INIFINITE 0xFFFFFFFF
    #define WRP_TIMEOUT_ERR ETIMEDOUT

#elif defined(__WIN32__)
    #include <windows.h>
    #define wrp_cond_t HANDLE
    #define WRP_TIMEOUT_INIFINITE INFINITE
    #define WRP_TIMEOUT_ERR WAIT_TIMEOUT

#else
    #error "Unsupported platform" // This check is done only once at the beginning of file for convenience
#endif

int wrp_cond_init(wrp_cond_t* cond);
int wrp_cond_destroy(wrp_cond_t* cond);

int wrp_cond_wait(wrp_cond_t* cond, wrp_mutex_t* mutex, unsigned long timeout_ms);
int wrp_cond_signal(wrp_cond_t* cond); // Signals only one thread (to support both unix and win)

#endif /* WRP_COND_H_ */