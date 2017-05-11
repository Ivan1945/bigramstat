#pragma once

#include <stdint.h>

typedef uint32_t ZBOOL;

//temp may be
# ifndef __int64
#  define __int64 long long
# endif


#define ZTRUE			1
#define ZFALSE			0


#ifdef __GNUC__
    //#define ZFORCEINLINE __attribute__((always_inline))
    #define ZFORCEINLINE inline
#else
    #define ZFORCEINLINE __forceinline
#endif // __GNUC__
