
#ifndef WRP_MUTEX_H_
#define WRP_MUTEX_H_

#include <assert.h> // #define NDEBUG will disable asserts in code

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <pthread.h>
    #define wrp_mutex_t pthread_mutex_t

#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #define wrp_mutex_t CRITICAL_SECTION

#else
    #error "Unsupported platform" // This check is done only once at the beginning of file for convenience
#endif

void wrp_mutex_init(wrp_mutex_t* mutex);
void wrp_mutex_destroy(wrp_mutex_t* mutex);

void wrp_mutex_lock(wrp_mutex_t* mutex);
void wrp_mutex_unlock(wrp_mutex_t* mutex);

#endif /* WRP_MUTEX_H_ */
