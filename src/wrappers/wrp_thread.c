
#include "wrp_thread.h"


void wrp_thread_create(wrp_thread_t* thread, wrp_thread_func_t func, void* arg, bool joinable) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    pthread_attr_t attr;
    if (!joinable) {
        ret = pthread_attr_init(&attr);
        assert(ret == 0);
        ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        assert(ret == 0);
    }

    ret = pthread_create(thread, NULL, func, arg);
    assert(ret == 0);

    if (!joinable) {
        ret = pthread_attr_destroy(&attr);
        assert(ret == 0);
    }

    #elif defined(__WIN32__)
    *thread = CreateThread(NULL, 0, func, arg, 0, NULL);
    assert(*thread != NULL);

    if (!joinable) {
        ret = CloseHandle(thread);
        assert(ret != 0);
    }
    #endif
}

void wrp_thread_join(wrp_thread_t thread, wrp_thread_ret_t* wrp_thread_ret) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_join(thread, wrp_thread_ret);
    assert(ret == 0);

    #elif defined(__WIN32__)
    ret = WaitForSingleObject(thread, INFINITE) != WAIT_OBJECT_0);
    assert(ret == WAIT_OBJECT_0);

    if (wrp_thread_ret != NULL) {
        ret = GetExitCodeThread(thread, wrp_thread_ret);
        assert(ret != 0);
    }

    ret = CloseHandle(thread);
    assert(ret != 0);
    #endif
}

void wrp_thread_terminate(wrp_thread_t thread) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = pthread_cancel(thread);
    assert(ret == 0);

    #elif defined(__WIN32__)
    ret = TerminateThread(thread, WRP_THREAD_TERMINATED);
    assert(ret != 0);
    
    ret = CloseHandle(thread);
    assert(ret != 0);
    #endif
}
