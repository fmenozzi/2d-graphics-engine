/*
 * Copyright (2015) Federico Menozzi
 */

#include <FedRadialGradientShader.h>
#include <FedUtil.h>

bool FedRadialGradientShader::setContext(const float ctm[6]) {
    float r  = m_radius;
    float cx = m_center.x();
    float cy = m_center.y();

    float local[] = {
        r, 0, cx,
        0, r, cy,
        0, 0,  1,
    };

    m_xform = (FedMatrix3x3(ctm) * FedMatrix3x3(local)).inv();

    return true;
}

/*
 * TODO: Figure out how to forward difference t calculation
 */
void FedRadialGradientShader::shadeRow(int x, int y, int count, GPixel row[]) {
    GPoint start  = GPoint::Make(x+0.5, y+0.5);
    GPoint lookup = m_xform.apply(start);

    GColor c0 = m_colors[0];
    GColor c1 = m_colors[1];

    float a = m_xform[0];
    float d = m_xform[3];

    for (int i = 0; i < count; i++) {
        float t = FedUtil::clamp(0.0f, sqrtf(lookup.x()*lookup.x() + lookup.y()*lookup.y()), 1.0f);
        GColor color = FedUtil::lerp(c0, t, c1);

        row[i] = FedUtil::toPixel(color);

        lookup.fX += a;
        lookup.fY += d;
    }
}
