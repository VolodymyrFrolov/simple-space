
#ifndef WRP_COMMON_H_
#define WRP_COMMON_H_

#if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    #include <dlfcn.h> // dlopen() dlsym()
    #include <unistd.h>
    #define wrp_library_handle void*
    #define wrp_load_library(name) dlopen(name, RTLD_NOW)
    #define wrp_sleep_ms(ms) usleep((ms)*1000)

#elif defined(__WIN32__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
    #define wrp_library_handle HMODULE
    #define wrp_load_library(name) LoadLibrary(name)
    #define wrp_sleep_ms(ms) Sleep(ms)

#else
    #error "Unsupported platform"
#endif

int wrp_load_function(void** p_func_prt, wrp_library_handle lib_handle, const char* func_name);
int wrp_close_library(wrp_library_handle lib_handle);

#endif /* WRP_COMMON_H_ */
