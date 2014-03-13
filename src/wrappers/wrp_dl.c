
#include "wrp_dl.h"

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

    return ret;
}

void wrp_close_library(wrp_library_handle lib_handle) {
    int ret;

    #if defined(__linux__) || defined(__APPLE__) || defined(__android__)
    ret = dlclose(lib_handle);
    assert(ret == 0);

    #elif defined(__WIN32__)
    ret = FreeLibrary(lib_handle);
    assert(ret != 0);
    #endif
}
