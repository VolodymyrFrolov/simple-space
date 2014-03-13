
#include "wrp_cond.h"

void wrp_cond_init(wrp_cond_t* cond) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_init(cond, NULL);
    assert(ret == 0);

    #elif defined(__WIN32__)
    *cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(*cond != NULL);
    #endif
}

void wrp_cond_destroy(wrp_cond_t* cond) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_destroy(cond);
    assert(ret == 0);

    #elif defined(__WIN32__)
    ret = CloseHandle(*cond);
    assert(ret != 0);
    #endif
}

int wrp_cond_wait(wrp_cond_t* cond, wrp_mutex_t* mutex, unsigned long timeout_ms) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    if (timeout_ms == WRP_TIMEOUT_INIFINITE) {
        ret = pthread_cond_wait(cond, mutex);
        assert(ret == 0);
    } else {
        struct timeval tv;
        struct timespec ts;

        gettimeofday(&tv, NULL);
        tv.tv_sec += timeout_ms/1000;
        suseconds_t new_usec = tv.tv_usec + (timeout_ms%1000) * 1000;
        if (new_usec < 1e6) {
            tv.tv_usec = new_usec;
        } else {
            tv.tv_sec++;
            tv.tv_usec = new_usec - 1e6;
        }

        ts.tv_sec = tv.tv_sec;
        ts.tv_nsec = tv.tv_usec * 1000;

        ret = pthread_cond_timedwait(cond, mutex, &ts);
        if ((ret != 0) && (ret != WRP_TIMEOUT_ERR)) {
            assert(0); // Error case
        }
    }

    #elif defined(__WIN32__)
    wrp_mutex_unlock(mutex);
    ret = WaitForSingleObject(*cond, timeout_ms);
    if ((ret != WAIT_OBJECT_0) && (ret != WRP_TIMEOUT_ERR)) {
        assert(0); // Error case
    }
    wrp_mutex_lock(mutex);
    #endif
    
    return ret;
}

void wrp_cond_signal(wrp_cond_t* cond) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_signal(cond);
    assert(ret == 0);
    
    #elif defined(__WIN32__)
    ret = SetEvent(*cond);
    assert(ret != 0);
    #endif
}
