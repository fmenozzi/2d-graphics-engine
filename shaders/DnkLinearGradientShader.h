/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKLINEARGRADIENTSHADER_H_
#define DNKLINEARGRADIENTSHADER_H_

#include <GShader.h>
#include <GPoint.h>
#include <GColor.h>
#include <GPixel.h>

#include <DnkMatrix3x3.h>

class DnkLinearGradientShader : public GShader {
public:
    DnkLinearGradientShader(const GPoint points[2], const GColor colors[2]);

    bool setContext(const float ctm[6]) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GPoint m_points[2];
    GColor m_colors[2];

    DnkMatrix3x3 m_xform;

    float m_D;
};

#endif  // DNKLINEARGRADIENTSHADER_H_
