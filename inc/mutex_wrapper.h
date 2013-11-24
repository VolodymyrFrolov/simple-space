
#ifndef MUTEX_WRAPPER_H_
#define MUTEX_WRAPPER_H_

#if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    #include <pthread.h>
    #define mutex_hdl pthread_mutex_t

#elif defined(__WIN32__)
    #include <windows.h>
    #include "process.h"
    #define mutex_hdl HANDLE

#else
    #error "Unsupported platform" // This check is done only once at the beginning of file for convenience
#endif

int mutex_init_wrapper(mutex_hdl* hdl);
int mutex_deinit_wrapper(mutex_hdl* hdl);

int mutex_lock_wrapper(mutex_hdl* hdl);
int mutex_unlock_wrapper(mutex_hdl* hdl);

#endif /* MUTEX_WRAPPER_H_ */