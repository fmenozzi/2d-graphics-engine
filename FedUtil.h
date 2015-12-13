/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FedUtil_DEFINED
#define FedUtil_DEFINED

#include <GPixel.h>

#include "FedMatrix3x3.h"

class GColor;
class GRect;
class GIRect;
class GBitmap;
class GPoint;

namespace FedUtil {
    GPixel blend(GPixel src, GPixel dst);
    GPixel toPixel(const GColor& color);
    GColor toColor(GPixel pixel);

    void mapRectToRect(const GRect& src, const GRect& dst, float res[9]);

    struct FedEdge {
        int top_y, bot_y;
        float m;
        float curr_x;

        FedEdge() {top_y = bot_y = m = curr_x = 0;}
        FedEdge(int ty, int by, float m, float cx) : top_y(ty), bot_y(by), m(m), curr_x(cx) {}
    };
    int numEdgesAfterClipping(GPoint p0, GPoint p1, int w);
    int pointsToEdges(const GPoint points[], int count, FedEdge edges[], int w, int h);
    bool byYThenX(FedEdge a, FedEdge b);

    void convertToQuad(const GRect& rect, GPoint pts[4]);
    FedMatrix3x3 mapRectToRectMat(const GRect& src, const GRect& dst);

    void blendRow(const GPixel src[], GPixel dst[], int count);
    template <typename T> T clamp(T min, T value, T max) {
        return std::max(min, std::min(value, max));
    }
    GColor lerp(const GColor& c0, float t, const GColor& c1);

    GBitmap bitmapSlice(const GBitmap& src, const GRect& slice);
    void divideIntoNine(const GBitmap& src, const GIRect& center, const GRect& dst, GRect corners_lrtb_center[9]);

}

#endif
