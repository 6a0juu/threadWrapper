#include <stdio.h>
#include <string.h>
#ifndef TW_SRC_PREDEF_C
    #define TW_SRC_PREDEF_C
/**
 *  * Determination a platform of an operation system
 *   * Fully supported supported only GNU GCC/G++, partially on Clang/LLVM
 *    */

    #if defined(_WIN32)
        #define PLATFORM_NAME "windows32" // Windows
    #elif defined(_WIN64)
        #define PLATFORM_NAME "windows64" // Windows
    #elif defined(__MINGW64__)
        #define PLATFORM_NAME "windowsmingw64"
    #elif defined(__MINGW32__)
        #define PLATFORM_NAME "windowsmingw32"
    #elif defined(__CYGWIN__) && !defined(_WIN32)
        #define PLATFORM_NAME "windowscyg" // Windows (Cygwin POSIX under Microsoft Window)
    #elif defined(__ANDROID__)
        #define PLATFORM_NAME "android" // Android (implies Linux, so it must come first)
    #elif defined(__linux__)
        #define PLATFORM_NAME "linux" // Debian, Ubuntu, Gentoo, Fedora, openSUSE, RedHat, Centos and other
    #elif defined(__unix__) || defined(__APPLE__) && defined(__MACH__)
        #include <sys/param.h>
        #if defined(BSD)
            #define PLATFORM_NAME "bsd" // FreeBSD, NetBSD, OpenBSD, DragonFly BSD
        #endif
    #elif defined(__APPLE__) && defined(__MACH__) // Apple OSX and iOS (Darwin)
        #include <TargetConditionals.h>
        #if TARGET_IPHONE_SIMULATOR == 1
            #define PLATFORM_NAME "ios" // Apple iOS
        #elif TARGET_OS_IPHONE == 1
            #define PLATFORM_NAME "ios" // Apple iOS
        #elif TARGET_OS_MAC == 1
            #define PLATFORM_NAME "osx" // Apple OSX
        #endif
    #else
        #define PLATFORM_NAME NULL
    #endif

    #if defined(__x86_64__)
        #define ARCH_NAME "x86_64"
    #elif defined(__i386__)
        #define ARCH_NAME "x86"
    #elif defined(__aarch64__)
        #define ARCH_NAME "AARCH_64"
    #elif defined(__arm__)
        #define ARCH_NAME "AARCH_32"
    #elif defined(__mips__)
        #define ARCH_NAME "MIPS"
    #else
        #define ARCH_NAME NULL
    #endif

#endif

// Return a name of platform, if determined, otherwise - an empty string
char *get_platform_name() {
     return (PLATFORM_NAME == NULL) ? "" : PLATFORM_NAME;
}

char *get_arch_name() {
     return (ARCH_NAME == NULL) ? "" : ARCH_NAME;
}
