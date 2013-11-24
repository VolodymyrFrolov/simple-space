
#include <stdio.h>
#include "thread_wrapper.h"

int thread_create_wrapper(thread_hdl* hdl, thread_func func, void* arg) {
    int ret = 0;

    #if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    ret = pthread_create(hdl, NULL, func, arg);

    #elif defined(__WIN32__)
    *hdl = CreateThread(NULL, 0, func, arg, 0, NULL);
    if (*hdl == NULL)
        ret = -1;
    #endif

    if (ret != 0)
        printf("thread_create_wrapper: ERROR\n");
    return ret;
}

int thread_join_wrapper(thread_hdl hdl, thread_ret* p_thread_ret) {
    int ret = 0;

    #if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    ret = pthread_join(hdl, p_thread_ret);

    #elif defined(__WIN32__)
    if (WaitForSingleObject(hdl, INFINITE) != WAIT_OBJECT_0) {
        printf("thread_join_wrapper error[A]: %ld\n", GetLastError());
        ret = -1;
    }
    if (GetExitCodeThread(hdl, p_thread_ret) == 0) {
        printf("thread_join_wrapper error[B]: %ld\n", GetLastError());
        ret = -1;
    }
    if (CloseHandle(hdl) == 0) {
        printf("thread_join_wrapper error[C]: %ld\n", GetLastError());
        ret = -1;
    }
    #endif

    if (ret != 0)
        printf("thread_join_wrapper: ERROR\n");
    return ret;
}

int thread_terminate_wrapper(thread_hdl hdl) {
    int ret = 0;

    #if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    ret = pthread_cancel(hdl);

    #elif defined(__WIN32__)
    if (TerminateThread(hdl, WAS_TERMINATED) == 0) {
        ret = -1;
    }
    #endif

    if (ret != 0)
        printf("thread_terminate_wrapper: ERROR\n");
    return ret;
}
