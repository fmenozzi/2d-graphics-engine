/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKLERPSHADER_H_
#define DNKLERPSHADER_H_

#include <GShader.h>
#include <GPixel.h>

#include <DnkMatrix3x3.h>
#include <DnkUtil.h>

class DnkLerpShader : public GShader {
public:
    DnkLerpShader(GShader* a, GShader* b, float t) {
        m_a = a;
        m_b = b;

        m_t = DnkUtil::clamp(0.0f, t, 1.0f);
    }

    bool setContext(const float ctm[6]) override;

    void shadeRow(int x, int y, int count, GPixel row[]) override;

private:
    GShader* m_a;
    GShader* m_b;

    float m_t;

    DnkMatrix3x3 m_xform;
};

#endif  // DNKLERPSHADER_H_
