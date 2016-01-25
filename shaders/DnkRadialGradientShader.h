/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKRADIALGRADIENTSHADER_H_
#define DNKRADIALGRADIENTSHADER_H_

#include <GShader.h>
#include <GPoint.h>
#include <GColor.h>

#include <DnkMatrix3x3.h>

class DnkRadialGradientShader : public GShader {
public:
    DnkRadialGradientShader(const GPoint& center, float radius, const GColor colors[2]) {
        m_center = center;
        m_radius = radius;

        m_colors[0] = colors[0];
        m_colors[1] = colors[1];
    }

    bool setContext(const float ctm[6]) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GPoint m_center;
    float  m_radius;
    GColor m_colors[2];
    DnkMatrix3x3 m_xform;
};

#endif  // DNKRADIALGRADIENTSHADER_H_
