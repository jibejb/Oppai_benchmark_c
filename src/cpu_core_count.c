#include "cpu_core_count.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
   //define something for Windows (32-bit and 64-bit, this part is common)
    #include <windows.h>
    long int cpu_core()
    {
        //系统信息
        SYSTEM_INFO sysInfo;
        //打印 CPU 核心数量
        GetSystemInfo(&sysInfo);
        return sysInfo.dwNumberOfProcessors;
    }
#elif defined(__linux__) || defined(__unix__) || defined(_POSIX_VERSION)
    // linux
    #include <unistd.h>
    long int cpu_core(){
        return sysconf( _SC_NPROCESSORS_CONF);
    }
#else
#   error "Unknown compiler"
#endif


