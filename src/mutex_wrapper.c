
#include <stdio.h>
#include "mutex_wrapper.h"

int mutex_init_wrapper(mutex_hdl* hdl) {
    int ret = 0;

    #if defined(__linux__) || defined (__android__) || defined(__APPLE__)
    ret = pthread_mutex_init(hdl, NULL);
    #elif defined(__WIN32__)
    *hdl = CreateMutex(NULL, FALSE, NULL);
    if (*hdl == NULL)
        ret = -1;
    #endif

    if (ret != 0)
        printf("mutex_init_wrapper: ERROR\n");
    return ret;
}

    int mutex_deinit_wrapper(mutex_hdl* hdl) {
    int ret = 0;

    #if defined(__linux__) || defined(__android__) || defined(__APPLE__)
    ret = pthread_mutex_destroy(hdl);
    #elif defined(__WIN32__)
    if (CloseHandle(*hdl) == 0) {
        printf("mutex_deinit_wrapper error: %ld\n", GetLastError());
        ret = -1;
        }
    #endif

    if (ret != 0)
        printf("mutex_deinit_wrapper: ERROR\n");
    return ret;
}

int mutex_lock_wrapper(mutex_hdl* hdl) {
    int ret = 0;

    #if defined(__linux__) || defined(__android__) || defined(__APPLE__)
    ret = pthread_mutex_lock(hdl);
    #elif defined(__WIN32__)
    if (WaitForSingleObject(*hdl, INFINITE) != WAIT_OBJECT_0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("mutex_lock_wrapper: ERROR\n");
    return ret;
}

int mutex_unlock_wrapper(mutex_hdl* hdl) {
    int ret = 0;

    #if defined(__linux__) || defined(__android__) || defined(__APPLE__)
    ret = pthread_mutex_unlock(hdl);

    #elif defined(__WIN32__)
    if(ReleaseMutex(*hdl) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("mutex_unlock_wrapper: ERROR\n");
    return ret;
}