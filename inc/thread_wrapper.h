
#ifndef THREAD_WRAPPER_H_
#define THREAD_WRAPPER_H_

#if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    #include <pthread.h>
    #define thread_hdl pthread_t
    #define thread_ret void*
    #define win_attr // windows specific attribute
    #define WAS_TERMINATED PTHREAD_CANCELED

#elif defined(__WIN32__)
    #include <windows.h>
    #include "process.h"
    #define thread_hdl HANDLE
    #define thread_ret DWORD
    #define win_attr WINAPI
    #define WAS_TERMINATED -1
    
#else
    #error "Unsupported platform" // This check is done only once at the beginning of file for convenience
#endif

typedef thread_ret win_attr (*thread_func)(void* arg);

int thread_create_wrapper(thread_hdl* hdl, thread_func func, void* arg);
int thread_join_wrapper(thread_hdl hdl, thread_ret* p_thread_ret);
int thread_terminate_wrapper(thread_hdl hdl); // Use this only in extreme situations

#endif /* THREAD_WRAPPER_H_ */