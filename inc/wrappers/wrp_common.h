
#ifndef WRP_COMMON_H_
#define WRP_COMMON_H_

#include <assert.h> // #define NDEBUG will disable asserts in code

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <unistd.h>
    #define wrp_sleep_ms(ms) usleep((ms)*1000)

#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #define wrp_sleep_ms(ms) Sleep(ms)

#else
    #error "Unsupported platform"
#endif

#endif /* WRP_COMMON_H_ */
