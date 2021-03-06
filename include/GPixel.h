/**
 *  Copyright 2015 Mike Reed
 */

#ifndef GPixel_DEFINED
#define GPixel_DEFINED

#include "GTypes.h"

#define GPIXEL_SHIFT_A  24
#define GPIXEL_SHIFT_R  16
#define GPIXEL_SHIFT_G   8
#define GPIXEL_SHIFT_B   0

/**
 *  Defines our 32bit pixel to be just an int. It stores its components based
 *  on the defines GPIXEL_SHIFT_... for each component.
 */
typedef uint32_t GPixel;

///////////////////////////////////////////////////////////////////////////////

static inline int GPixel_GetA(GPixel p) { return (p >> GPIXEL_SHIFT_A) & 0xFF; }
static inline int GPixel_GetR(GPixel p) { return (p >> GPIXEL_SHIFT_R) & 0xFF; }
static inline int GPixel_GetG(GPixel p) { return (p >> GPIXEL_SHIFT_G) & 0xFF; }
static inline int GPixel_GetB(GPixel p) { return (p >> GPIXEL_SHIFT_B) & 0xFF; }

/*
 *  Asserts that a, r, g, b are already in premultiply form, and simiply
 *  packs them into a GPixel.
 */
static inline GPixel GPixel_PackARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b) {
    GASSERT(a <= 255, "GPixel_PackARGB(): a is more than 255");
    GASSERT(r <= a,   "GPixel_PackARGB(): r is greater than a");
    GASSERT(g <= a,   "GPixel_PackARGB(): g is greater than a");
    GASSERT(b <= a,   "GPixel_PackARGB(): b is greater than a");
    
    return  (a << GPIXEL_SHIFT_A) |
            (r << GPIXEL_SHIFT_R) |
            (g << GPIXEL_SHIFT_G) |
            (b << GPIXEL_SHIFT_B);
}

#endif
