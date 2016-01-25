/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKBITMAPSHADER_H_
#define DNKBITMAPSHADER_H_

#include <GShader.h>
#include <GBitmap.h>

#include <DnkMatrix3x3.h>

class DnkBitmapShader : public GShader {
public:
    DnkBitmapShader(const GBitmap& src, const float localMatrix[6]) :
        m_src(src),
        m_local(DnkMatrix3x3(localMatrix)) {}

    bool setContext(const float ctm[6]) override {
        m_xform = (DnkMatrix3x3(ctm) * m_local).inv();
        return true;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GBitmap m_src;
    DnkMatrix3x3 m_local;
    DnkMatrix3x3 m_xform;
};

#endif  // DNKBITMAPSHADER_H_
