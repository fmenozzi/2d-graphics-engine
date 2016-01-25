/**
 * Copyright 2015 Federico Menozzi
 */

#ifndef FEDBITMAP_H_
#define FEDBITMAP_H_

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
    FedBitmap(const std::string& path) {
        readFromFile(path.c_str());
    }

    ~FedBitmap() {
        if (fPixels)
            free(fPixels);
    }
};

#endif
