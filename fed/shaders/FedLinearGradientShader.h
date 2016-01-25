/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FEDLINEARGRADIENTSHADER_H_
#define FEDLINEARGRADIENTSHADER_H_

#include <GShader.h>
#include <GPoint.h>
#include <GColor.h>
#include <GPixel.h>

#include <FedMatrix3x3.h>

class FedLinearGradientShader : public GShader {
public:
    FedLinearGradientShader(const GPoint points[2], const GColor colors[2]);

    bool setContext(const float ctm[6]) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GPoint m_points[2];
    GColor m_colors[2];

    FedMatrix3x3 m_xform;

    float m_D;
};

#endif  // FEDLINEARGRADIENTSHADER_H_
