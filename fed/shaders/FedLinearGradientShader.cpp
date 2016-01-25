/*
 * Copyright (2015) Federico Menozzi
 */

#include <FedLinearGradientShader.h>
#include <FedUtil.h>

FedLinearGradientShader::FedLinearGradientShader(const GPoint points[2], const GColor colors[2]) {
    m_points[0] = points[0];
    m_points[1] = points[1];

    m_colors[0] = colors[0];
    m_colors[1] = colors[1];

    float dx = points[1].x() - points[0].x();
    float dy = points[1].y() - points[0].y();
    m_D = sqrt(dx*dx + dy*dy);
}

bool FedLinearGradientShader::setContext(const float ctm[6]) {
    float p0x = m_points[0].x();
    float p0y = m_points[0].y();

    float dx = m_points[1].x() - p0x;
    float dy = m_points[1].y() - p0y;

    float local[] = {
        dx, -dy, p0x,
        dy,  dx, p0y,
         0,   0,   1,
    };

    m_xform = (FedMatrix3x3(ctm) * FedMatrix3x3(local)).inv();

    return true;
}

void FedLinearGradientShader::shadeRow(int x, int y, int count, GPixel row[]) {
    GPoint start  = GPoint::Make(x+0.5, y+0.5);
    GPoint lookup = m_xform.apply(start);

    GColor c0 = m_colors[0];
    GColor c1 = m_colors[1];

    float a = m_xform[0];
    float d = m_xform[3];

    for (int i = 0; i < count; i++) {
        float t = FedUtil::clamp(0.0f, lookup.x(), 1.0f);
        GColor color = FedUtil::lerp(c0, t, c1);

        row[i] = FedUtil::toPixel(color);

        lookup.fX += a;
        lookup.fY += d;
    }
}
