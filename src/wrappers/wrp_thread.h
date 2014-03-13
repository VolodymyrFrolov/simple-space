
#ifndef WRP_THREAD_H_
#define WRP_THREAD_H_

#include <stdbool.h>
#include <assert.h> // #define NDEBUG will disable asserts in code

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <pthread.h>
    #define wrp_thread_t pthread_t
    #define wrp_thread_ret_t void*
    #define win_attr // win-specific attribute
    #define WRP_THREAD_TERMINATED PTHREAD_CANCELED

#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #include "process.h"
    #define wrp_thread_t HANDLE
    #define wrp_thread_ret_t DWORD
    #define win_attr WINAPI
    #define WRP_THREAD_TERMINATED 0xffffffff

#else
    #error "Unsupported platform" // This check is done only once at the beginning of file for convenience
#endif

typedef wrp_thread_ret_t win_attr (*wrp_thread_func_t)(void* arg);

void wrp_thread_create(wrp_thread_t* thread, wrp_thread_func_t func, void* arg, bool joinable);
void wrp_thread_join(wrp_thread_t thread, wrp_thread_ret_t* wrp_thread_ret);
void wrp_thread_terminate(wrp_thread_t thread); // Use this only, when no choice left

#endif /* WRP_THREAD_H_ */
