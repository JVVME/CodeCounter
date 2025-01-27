#if defined(_WIN32) || defined(_WIN64)
    #define FCONFIG_WINDOWS 1
    #define FCONFIG_LINUX 0
    #include <windows.h>
#elif defined(__linux__)
    #define FCONFIG_WINDOWS 0
    #define FCONFIG_LINUX 1
    #include <dirent.h>
    #include <sys/stat.h>
    #include <stdlib.h>
    #include <sys/time.h>
#else
    #define FCONFIG_WINDOWS 0
    #define FCONFIG_LINUX 0
#endif