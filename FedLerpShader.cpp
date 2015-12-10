#include "FedLerpShader.h"

#include <GColor.h>

bool FedLerpShader::setContext(const float ctm[6]) {
    m_a->setContext(ctm);
    m_b->setContext(ctm);

    m_xform = FedMatrix3x3(ctm).inv();

    return true;
}

void FedLerpShader::shadeRow(int x, int y, int count, GPixel row[]) {
    GPoint start  = GPoint::Make(x+0.5, y+0.5);
    GPoint lookup = m_xform.apply(start);

    float t = m_t;

    float a = m_xform[0];
    float d = m_xform[3];

    GPixel* arow = new GPixel[count];
    GPixel* brow = new GPixel[count];

    m_a->shadeRow(x, y, count, arow);
    m_b->shadeRow(x, y, count, brow);

    for (int i = 0; i < count; i++) {
        GColor c0 = FedUtil::toColor(arow[i]);
        GColor c1 = FedUtil::toColor(brow[i]);
    
        GColor color = FedUtil::lerp(c0, t, c1);

        row[i] = FedUtil::toPixel(color);

        lookup.fX += a;
        lookup.fY += d;
    }

    delete[] arow;
    delete[] brow;
}
