/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FedLerpShader_DEFINED
#define FedLerpShader_DEFINED

#include <GShader.h>
#include <GPixel.h>

#include "FedMatrix3x3.h"
#include "FedUtil.h"

class FedLerpShader : public GShader {
public:
    FedLerpShader(GShader* a, GShader* b, float t) {
        m_a = a;
        m_b = b;

        m_t = FedUtil::clamp(0.0f, t, 1.0f);
    }

    bool setContext(const float ctm[6]) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GShader* m_a;
    GShader* m_b;

    float m_t;

    FedMatrix3x3 m_xform;
};

#endif
