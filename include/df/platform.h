#pragma once

#define DF_VERSION_MAJOR 0
#define DF_VERSION_MINOR 1

// Platform detection OS
#if defined( _WIN64 )
    #define DF_PLATFORM_WIN    
#elif defined(_WIN32) || defined(__WIN32__)
    #define DF_PLATFORM_WIN
#elif __ANDROID__
    #define DF_PLATFORM_ANDROID
#elif __APPLE__
    #include "TargetConditionals.h"
    #ifdef TARGET_OS_IPHONE
        #define DF_PLATFORM_IOS
    #elif TARGET_IPHONE_SIMULATOR
        #define DF_PLATFORM_IOS_SIM
    #elif TARGET_OS_MAC
        #define DF_PLATFORM_OSX
    #else
        #error Unknown Apple platform.
    #endif
#elif defined(linux) || defined(__linux)
    #define DF_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
    #define DF_PLATFORM_FREEBSD
#else
    // Unsupported system
    #error Unknown platform.
#endif


// Debug build detection
#if !defined(NDEBUG)
    #define DF_DEBUG
#endif

//Compiler specific macro
#if defined( _MSC_VER )
    #define DF_COMPILER_MSVC
    #define DF_API_EXPORT __declspec(dllexport)
    #define DF_API_IMPORT __declspec(dllimport)
    
    #define DF_ALIGN_PRE( ALIGNMENT ) __declspec( align( ALIGNMENT ) )
    #define DF_ALIGN_POST( ALIGNMENT )
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #pragma warning( disable : 4251 )//Template classes shouldn't be DLL exported, but the compiler warns us by default
    #ifndef _CRT_SECURE_NO_WARNINGS
        #define _CRT_SECURE_NO_WARNINGS
    #endif
#elif defined( __GNUC__ )
    #define DF_COMPILER_GCC
    #if __GNUC__ >= 4
        // GCC 4 has special keywords for showing/hidding symbols
        #define DF_API_EXPORT __attribute__ ((__visibility__ ("default")))
        #define DF_API_IMPORT __attribute__ ((__visibility__ ("default")))
    #else
       // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
        #define DF_API_EXPORT
        #define DF_API_IMPORT
    #endif

    //force a variable to be aligned in memory. ALIGNMENT must be a power of two.
    #define DF_ALIGN_PRE( ALIGNMENT )
    #define DF_ALIGN_POST( ALIGNMENT ) __attribute__( ( aligned( ALIGNMENT ) ) ) 
#else
    #error Unknown compiler.
#endif

// portable fixed-size types
namespace df
{
    typedef signed   char int8;
    typedef unsigned char uint8;
    typedef signed   short int16;
    typedef unsigned short uint16;
    typedef signed   int int32;
    typedef unsigned int uint32;
    #if defined(DF_COMPILER_MSVC)
        typedef signed   __int64 int64;
        typedef unsigned __int64 uint64;
    #else
        typedef signed   long long int64;
        typedef unsigned long long uint64;
    #endif
}
