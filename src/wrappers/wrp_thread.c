
#include <stdio.h>
#include "wrp_thread.h"

int wrp_thread_create(wrp_thread_t* thread, wrp_thread_func_t func, void* arg) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_create(thread, NULL, func, arg);

    #elif defined(__WIN32__)
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
    if (*thread == NULL)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_thread_create: ERROR\n");
    return ret;
}

int wrp_thread_join(wrp_thread_t thread, wrp_thread_ret_t* wrp_thread_ret) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_join(thread, wrp_thread_ret);
    if (ret != 0) {
        printf("wrp_thread_join: ERROR\n");
    }
    return ret;

    #elif defined(__WIN32__)
    ret = WaitForSingleObject(thread, INFINITE);
    if (ret != WAIT_OBJECT_0) {
        printf("wrp_thread_join: ERROR\n");
        return ret;
    }

    if (wrp_thread_ret != NULL) {
        if (GetExitCodeThread(thread, wrp_thread_ret) == 0) {
            printf("wrp_thread_join: ERROR\n");
            return -1;
        }
    }

    if (CloseHandle(thread) == 0) {
        printf("wrp_thread_join: ERROR\n");
        ret = -1;
    }

    return ret;
    #endif
}

int wrp_thread_terminate(wrp_thread_t thread) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cancel(thread);

    #elif defined(__WIN32__)
    if (TerminateThread(thread, WRP_THREAD_TERMINATED) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_thread_terminate: ERROR\n");
    return ret;
}
