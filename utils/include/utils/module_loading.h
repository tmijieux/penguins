#ifndef PENGUINS_MODULE_LOADING_H
#define PENGUINS_MODULE_LOADING_H

#ifdef _WIN32
# include <Windows.h>
#else
# include <dlfcn.h>
#endif

#ifdef _WIN32
# define peng_libhandle_t HINSTANCE
# define peng_load_library(x) LoadLibrary((x))
# define peng_get_proc_address(handle, name)  GetProcAddress((handle), (name))
# define peng_free_library(handle) FreeLibrary((handle))
# define peng_lib_handle_error(type, msg) printf("Error when loading %s : %s", (type), (msg))
# define PENG_MODULE_EXTENSION ".dll"
#else
# define peng_libhandle_t void*
# define peng_load_library(x) dlopen((x), RTLD_NOW)
# define peng_get_proc_address(handle, name) dlsym((handle), (name))
# define peng_free_library(handle) dlclose((handle))
# define peng_lib_handle_error(type, msg) puts(dlerror())
# define PENG_MODULE_EXTENSION ".so"

#endif


#endif // PENGUINS_MODULE_LOADING_H
