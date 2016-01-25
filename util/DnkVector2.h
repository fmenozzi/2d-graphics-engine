/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef DNKVECTOR2_H_
#define DNKVECTOR2_H_

#include <GPoint.h>

#include <cmath>

class DnkVector2 {
public:
    DnkVector2(float v0, float v1) {
        m_v0 = v0;
        m_v1 = v1;

        m_mag = sqrtf(m_v0*m_v0 + m_v1*m_v1);
    }
    DnkVector2(const GPoint& p0, const GPoint& p1) {
        m_v0 = p1.x() - p0.x();
        m_v1 = p1.y() - p0.y();

        m_mag = sqrtf(m_v0*m_v0 + m_v1*m_v1);
    }

    float v0() const { return m_v0; }
    float v1() const { return m_v1; }

    float mag() const { return m_mag; }

    DnkVector2 unit() const {
        float inv_mag = 1 / m_mag;

        return DnkVector2(m_v0 * inv_mag, m_v1 * inv_mag);
    }

    DnkVector2 scaled(float scale) const {
        DnkVector2 u = unit();
        return DnkVector2(u.v0() * scale, u.v1() * scale);
    }

    DnkVector2 rotate90ccw() const {
        float new_v0 =  m_v1;
        float new_v1 = -m_v0;

        return DnkVector2(new_v0, new_v1);
    }
    DnkVector2 rotate90cw() const {
        float new_v0 = -m_v1;
        float new_v1 =  m_v0;

        return DnkVector2(new_v0, new_v1);
    }

    float dot(const DnkVector2& other) {
        return m_v0*other.m_v0 + m_v1*other.m_v1;
    }

    float crossmag(const DnkVector2& other) {
        return m_v0*other.m_v1 - m_v1*other.m_v0;
    }

    DnkVector2 operator+(const DnkVector2& other) {
        return DnkVector2(m_v0 + other.m_v0, m_v1 + other.m_v1);
    }

    DnkVector2 operator-(const DnkVector2& other) {
        return DnkVector2(m_v0 - other.m_v0, m_v1 - other.m_v1);
    }

    DnkVector2 operator-() {
        return DnkVector2(-m_v0, -m_v1);
    }

    DnkVector2& operator=(const DnkVector2& other) {
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

GPoint operator+(const GPoint& p, const DnkVector2& v) {
    return GPoint::Make(p.x() + v.v0(), p.y() + v.v1());
}

GPoint operator-(const GPoint& p, const DnkVector2& v) {
    return GPoint::Make(p.x() - v.v0(), p.y() - v.v1());
}

#endif  // DNKVECTOR2_H_
