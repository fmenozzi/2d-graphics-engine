/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FedVector2_DEFINED
#define FedVector2_DEFINED

#include <GPoint.h>

#include <cmath>

class FedVector2 {
public:
    FedVector2(float v0, float v1) {
        m_v0 = v0;
        m_v1 = v1;

        m_mag = sqrtf(m_v0*m_v0 + m_v1*m_v1);
    }
    FedVector2(const GPoint& p0, const GPoint& p1) {
        m_v0 = p1.x() - p0.x();
        m_v1 = p1.y() - p0.y();

        m_mag = sqrtf(m_v0*m_v0 + m_v1*m_v1);
    }

    float v0() const { return m_v0; }
    float v1() const { return m_v1; }

    float mag() const { return m_mag; }

    FedVector2 unit() const {
        float inv_mag = 1 / m_mag;

        return FedVector2(m_v0 * inv_mag, m_v1 * inv_mag);
    }

    FedVector2 scaled(float scale) const {
        FedVector2 u = unit();
        return FedVector2(u.v0() * scale, u.v1() * scale);
    }

    FedVector2 rotate90ccw() const {
        float new_v0 =  m_v1;
        float new_v1 = -m_v0;

        return FedVector2(new_v0, new_v1);
    }
    FedVector2 rotate90cw() const {
        float new_v0 = -m_v1;
        float new_v1 =  m_v0;

        return FedVector2(new_v0, new_v1);
    }

    float dot(const FedVector2& other) {
        return m_v0*other.m_v0 + m_v1*other.m_v1;
    }

    float crossmag(const FedVector2& other) {
        return m_v0*other.m_v1 - m_v1*other.m_v0;
    }

    FedVector2 operator+(const FedVector2& other) {
        return FedVector2(m_v0 + other.m_v0, m_v1 + other.m_v1);
    }

    FedVector2 operator-(const FedVector2& other) {
        return FedVector2(m_v0 - other.m_v0, m_v1 - other.m_v1);
    }

    FedVector2 operator-() {
        return FedVector2(-m_v0, -m_v1);
    }

    FedVector2& operator=(const FedVector2& other) {
        if (this != &other) {
            m_v0 = other.m_v0;
            m_v1 = other.m_v1;
        }
        return *this;
    }

private:
    float m_v0, m_v1;
    float m_mag;
};

GPoint operator+(const GPoint& p, const FedVector2& v) {
    return GPoint::Make(p.x() + v.v0(), p.y() + v.v1());
}

GPoint operator-(const GPoint& p, const FedVector2& v) {
    return GPoint::Make(p.x() - v.v0(), p.y() - v.v1());
}

#endif
