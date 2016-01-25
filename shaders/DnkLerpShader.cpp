/*
 * Copyright 2015 Federico Menozzi
 */

#include <DnkLerpShader.h>

#include <GColor.h>

bool DnkLerpShader::setContext(const float ctm[6]) {
    m_a->setContext(ctm);
    m_b->setContext(ctm);

    m_xform = DnkMatrix3x3(ctm).inv();

    return true;
}

void DnkLerpShader::shadeRow(int x, int y, int count, GPixel row[]) {
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
        GColor ca = DnkUtil::toColor(arow[i]);
        GColor cb = DnkUtil::toColor(brow[i]);

        GColor color = DnkUtil::lerp(ca, t, cb);

        row[i] = DnkUtil::toPixel(color);

        lookup.fX += a;
        lookup.fY += d;
    }

    delete[] arow;
    delete[] brow;
}
