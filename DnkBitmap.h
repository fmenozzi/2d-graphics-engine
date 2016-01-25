/**
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKBITMAP_H_
#define DNKBITMAP_H_

#include <GBitmap.h>
#include <GPixel.h>

class DnkBitmap : public GBitmap {
public:
    DnkBitmap() {}
    DnkBitmap(int w, int h) {
        fWidth    = w;
        fHeight   = h;
        fRowBytes = w * sizeof(GPixel);
        fPixels   = (GPixel*)calloc(fRowBytes, h);
    }
    DnkBitmap(const std::string& path) {
        readFromFile(path.c_str());
    }

    ~DnkBitmap() {
        if (fPixels)
            free(fPixels);
    }
};

#endif
