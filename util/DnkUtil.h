/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKUTIL_H_
#define DNKUTIL_H_

#include <algorithm>

#include <GPixel.h>

#include "DnkMatrix3x3.h"

class GColor;
class GRect;
class GIRect;
class GBitmap;
class GPoint;

namespace DnkUtil {

GPixel blend(GPixel src, GPixel dst);
GPixel toPixel(const GColor& color);
GColor toColor(GPixel pixel);

void mapRectToRect(const GRect& src, const GRect& dst, float res[9]);

struct DnkEdge {
    int top_y, bot_y;
    float m;
    float curr_x;

    DnkEdge() {top_y = bot_y = m = curr_x = 0;}
    DnkEdge(int ty, int by, float m, float cx) : top_y(ty), bot_y(by), m(m), curr_x(cx) {}
};
int numEdgesAfterClipping(GPoint p0, GPoint p1, int w);
int pointsToEdges(const GPoint points[], int count, DnkEdge edges[], int w, int h);
bool byYThenX(DnkEdge a, DnkEdge b);

void convertToQuad(const GRect& rect, GPoint pts[4]);
DnkMatrix3x3 mapRectToRectMat(const GRect& src, const GRect& dst);

void blendRow(const GPixel src[], GPixel dst[], int count);
template <typename T> T clamp(T min, T value, T max) {
    return std::max(min, std::min(value, max));
}
GColor lerp(const GColor& c0, float t, const GColor& c1);

GBitmap bitmapSlice(const GBitmap& src, const GRect& slice);
void divideIntoNine(const GBitmap& src, const GIRect& center, const GRect& dst, GRect corners_lrtb_center[9]);

}  // namespace DnkUtil

#endif  // DNKUTIL_H_
