/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FedBitmapShader_DEFINED
#define FedBitmapShader_DEFINED

#include <GShader.h>
#include <GBitmap.h>

#include "FedMatrix3x3.h"

class FedBitmapShader : public GShader {
public:
    FedBitmapShader(const GBitmap& src, const float localMatrix[6]) :
        m_src(src),
        m_local(FedMatrix3x3(localMatrix)) {}

    bool setContext(const float ctm[6]) override {
        m_xform = (FedMatrix3x3(ctm) * m_local).inv();
        return true;
    }

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GBitmap m_src;
    FedMatrix3x3 m_local;
    FedMatrix3x3 m_xform;
};

#endif
