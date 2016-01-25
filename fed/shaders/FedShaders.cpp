/*
 * Copyright 2015 Federico Menozzi
 */

#include <GPoint.h>
#include <GColor.h>

#include <FedBitmapShader.h>
#include <FedLinearGradientShader.h>
#include <FedRadialGradientShader.h>
#include <FedLerpShader.h>

class GShader;
class GBitmap;

GShader* GShader::FromBitmap(const GBitmap& bitmap, const float localMatrix[6]) {
    return new FedBitmapShader(bitmap, localMatrix);
}

GShader* GShader::FromLinearGradient(const GPoint points[2], const GColor colors[2]) {
    return new FedLinearGradientShader(points, colors);
}

GShader* GShader::FromRadialGradient(const GPoint& center, float radius, const GColor colors[2]) {
    return new FedRadialGradientShader(center, radius, colors);
}

GShader* GShader::FromLerp(GShader* a, GShader* b, float t) {
    return new FedLerpShader(a, b, t);
}
