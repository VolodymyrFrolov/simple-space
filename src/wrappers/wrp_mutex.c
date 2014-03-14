
#include "wrp_mutex.h"

void wrp_mutex_init(wrp_mutex_t* mutex) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = pthread_mutex_init(mutex, NULL);
    assert(ret == 0);
    #elif defined(__WIN32__)
    InitializeCriticalSection(mutex);
    #endif
}

void wrp_mutex_destroy(wrp_mutex_t* mutex) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = pthread_mutex_destroy(mutex);
    assert(ret == 0);
    #elif defined(__WIN32__)
    DeleteCriticalSection(mutex);
    #endif
}

void wrp_mutex_lock(wrp_mutex_t* mutex) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = pthread_mutex_lock(mutex);
    assert(ret == 0);
    #elif defined(__WIN32__)
    EnterCriticalSection(mutex);
    #endif
}

void wrp_mutex_unlock(wrp_mutex_t* mutex) {
    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    int ret = pthread_mutex_unlock(mutex);
    assert(ret == 0);
    #elif defined(__WIN32__)
    LeaveCriticalSection(mutex);
    #endif
}
