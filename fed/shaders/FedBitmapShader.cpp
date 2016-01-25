/*
 * Copyright 2015 Federico Menozzi
 */

#include <FedBitmapShader.h>
#include <FedUtil.h>

void FedBitmapShader::shadeRow(int x, int y, int count, GPixel row[]) {
    GPoint start  = GPoint::Make(x+0.5, y+0.5);
    GPoint lookup = m_xform.apply(start);

    int w_1 = m_src.width() - 1;
    int h_1 = m_src.height() - 1;

    float a = m_xform[0];
    float d = m_xform[3];

    for (int i = 0; i < count; i++) {
        int lookup_x = FedUtil::clamp(0, (int)lookup.x(), w_1);
        int lookup_y = FedUtil::clamp(0, (int)lookup.y(), h_1);

        row[i] = *(m_src.getAddr(lookup_x, lookup_y));

        lookup.fX += a;
        lookup.fY += d;
    }
}
