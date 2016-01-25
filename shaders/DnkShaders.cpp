/*
 * Copyright 2015 Federico Menozzi
 */

#include <GPoint.h>
#include <GColor.h>

#include <DnkBitmapShader.h>
#include <DnkLinearGradientShader.h>
#include <DnkRadialGradientShader.h>
#include <DnkLerpShader.h>

class GShader;
class GBitmap;

GShader* GShader::FromBitmap(const GBitmap& bitmap, const float localMatrix[6]) {
    return new DnkBitmapShader(bitmap, localMatrix);
}

GShader* GShader::FromLinearGradient(const GPoint points[2], const GColor colors[2]) {
    return new DnkLinearGradientShader(points, colors);
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2]) {
    return new DnkRadialGradientShader(center, radius, colors);
}

GShader* GShader::FromLerp(GShader* a, GShader* b, float t) {
    return new DnkLerpShader(a, b, t);
}
