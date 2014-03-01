
#include <stdio.h>
#include "wrp_mutex.h"

int wrp_mutex_init(wrp_mutex_t* mutex) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_mutex_init(mutex, NULL);
    #elif defined(__WIN32__)
    InitializeCriticalSection(mutex);
    #endif

    if (ret != 0)
        printf("wrp_mutex_init: ERROR\n");
    return ret;
}

int wrp_mutex_destroy(wrp_mutex_t* mutex) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_mutex_destroy(mutex);
    #elif defined(__WIN32__)
    DeleteCriticalSection(mutex);
    #endif

    if (ret != 0)
        printf("wrp_mutex_destroy: ERROR\n");
    return ret;
}

int wrp_mutex_lock(wrp_mutex_t* mutex) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_mutex_lock(mutex);
    #elif defined(__WIN32__)
    EnterCriticalSection(mutex);
    #endif

    if (ret != 0)
        printf("wrp_mutex_lock: ERROR\n");
    return ret;
}

int wrp_mutex_unlock(wrp_mutex_t* mutex) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_mutex_unlock(mutex);
    #elif defined(__WIN32__)
    LeaveCriticalSection(mutex);
    #endif

    if (ret != 0)
        printf("mutex_unlock_wrapper: ERROR\n");
    return ret;
}
