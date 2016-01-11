/**
 * Copyright 2015 Federico Menozzi
 */

#ifndef
#define

#include <GBitmap.h>
#include <GPixel.h>

class FedBitmap : public GBitmap {
public:
    FedBitmap() {}
    FedBitmap(int w, int h) {
        fWidth    = w;
        fHeight   = h;
        fRowBytes = w * sizeof(GPixel);
        fPixels   = (GPixel*)calloc(fRowBytes, h);
    }

    ~FedBitmap() {
        if (fPixels)
            free(fPixels);
    }
}

#endif
