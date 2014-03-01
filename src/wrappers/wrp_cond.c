
#include <stdio.h>
#include "wrp_cond.h"

int wrp_cond_init(wrp_cond_t* cond) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_init(cond, NULL);

    #elif defined(__WIN32__)
    *cond = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (*cond == NULL)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_cond_init: ERROR\n");
    return ret;
}

int wrp_cond_destroy(wrp_cond_t* cond) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_destroy(cond);

    #elif defined(__WIN32__)
    if (CloseHandle(*cond) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_cond_destroy: ERROR\n");
    return ret;
}

int wrp_cond_wait(wrp_cond_t* cond, wrp_mutex_t* mutex, unsigned long timeout_ms) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    if (timeout_ms == WRP_TIMEOUT_INIFINITE) {
        ret = pthread_cond_wait(cond, mutex);
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
    }

    if ((ret != 0) && (ret != WRP_TIMEOUT_ERR)) {
        printf("wrp_cond_wait: ERROR\n");
    }
    return ret;

    #elif defined(__WIN32__)
    ret = wrp_mutex_unlock(mutex);
    if (ret != 0) {
        printf("wrp_cond_wait: ERROR\n");
        return ret;
    }

    ret = WaitForSingleObject(*cond, timeout_ms);

    if ((ret != WAIT_OBJECT_0) && (ret != WRP_TIMEOUT_ERR)) {
        printf("wrp_cond_wait: ERROR\n");
        return ret;
    }

    // Overwrite ret only in lock error case
    // to return success/timeout status of WaitForSingleObject
    if (wrp_mutex_lock(mutex) != 0) {
        ret = -1;
    }

    if ((ret != 0) && (ret != WRP_TIMEOUT_ERR)) {
        printf("wrp_cond_wait: ERROR\n");
    }

    return ret;
    #endif
}

int wrp_cond_signal(wrp_cond_t* cond) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cond_signal(cond);
    
    #elif defined(__WIN32__)
    if (SetEvent(*cond) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_cond_signal: ERROR\n");
    return ret;
}
