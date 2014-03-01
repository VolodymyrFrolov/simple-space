#ifndef WRP_THREAD_H_
#define WRP_THREAD_H_

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <pthread.h>
    #define wrp_thread_t pthread_t
    #define wrp_thread_ret_t void*
    #define win_attr // windows specific attribute
    #define WRP_THREAD_TERMINATED PTHREAD_CANCELED

#elif defined(__WIN32__)
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

int wrp_thread_create(wrp_thread_t* thread, wrp_thread_func_t func, void* arg);
int wrp_thread_join(wrp_thread_t thread, wrp_thread_ret_t* wrp_thread_ret);
int wrp_thread_terminate(wrp_thread_t thread); // Use this only in extreme situations

#endif /* WRP_THREAD_H_ */