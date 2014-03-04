
#include <stdio.h>
#include "wrp_common.h"

int wrp_load_function(void** p_func_prt, wrp_library_handle lib_handle, const char* func_name) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    char* err;
    *p_func_prt = dlsym(lib_handle, func_name);
    if ((err = dlerror()) != NULL)
        ret = -1;

    #elif defined(__WIN32__)
    *p_func_prt = GetProcAddress(lib_handle, func_name);
    if (*p_func_prt == NULL)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_load_function: Error\n");

    return ret;
}

int wrp_close_library(wrp_library_handle lib_handle) {
    int ret = 0;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    if (dlclose(lib_handle) != 0)
        ret = -1;

    #elif defined(__WIN32__)
    if (FreeLibrary(lib_handle) == 0)
        ret = -1;
    #endif

    if (ret != 0)
        printf("wrp_close_library: Error\n");

    return ret;
}
