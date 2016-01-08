/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GTypes_DEFINED
#define GTypes_DEFINED

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void g_crash(const char* msg) {
    fprintf(stderr, "g_crash called: %s\n", msg);
    *(int*)0x50FF8001 = 12345;
}

#ifdef NDEBUG
    #define GRELEASE
    #define GASSERT(pred)
    #define GDEBUGCODE(code)
#else
    #define GDEBUG
    #define GASSERT(pred, msg)  do { if (!(pred)) g_crash(msg); } while (0)
    #define GDEBUGCODE(code)    code
#endif

/**
 *  Given an array (not a pointer), this macro will return the number of
 *  elements declared in that array.
 */
#define GARRAY_COUNT(array) (int)(sizeof(array) / sizeof(array[0]))

#endif
