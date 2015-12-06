/*
 * Copyright 2015 Federico Menozzi
 */

#include "FedMatrix3x3.h"

GPoint FedMatrix3x3::apply(const GPoint& point) const {
    float a = m_mat[0], b = m_mat[1], c = m_mat[2];
    float d = m_mat[3], e = m_mat[4], f = m_mat[5];

    float x = point.x();
    float y = point.y();

    float xp = a*x + b*y + c;
    float yp = d*x + e*y + f;

    return GPoint::Make(xp,yp);
}

void FedMatrix3x3::apply(const GPoint points[], int count, GPoint xform_points[]) const {
    for (int i = 0; i < count; i++)
        xform_points[i] = apply(points[i]);
}

FedMatrix3x3 FedMatrix3x3::inv() const {
    float res[6];

    float a = m_mat[0], b = m_mat[1], c = m_mat[2];
    float d = m_mat[3], e = m_mat[4], f = m_mat[5];

    float ae = a*e, bd = b*d;
    float aebd_inv = 1 / (ae - bd);
    float bdae_inv = 1 / (bd - ae);

    res[0] = aebd_inv * e;
    res[1] = bdae_inv * b;
    res[2] = bdae_inv * (c*e - b*f);
    res[3] = bdae_inv * d;
    res[4] = aebd_inv * a;
    res[5] = aebd_inv * (c*d - a*f);

    return FedMatrix3x3(res);
}

FedMatrix3x3 FedMatrix3x3::operator*(const FedMatrix3x3& other) const {
    float res[6];

    float a = m_mat[0], b = m_mat[1], c = m_mat[2];
    float d = m_mat[3], e = m_mat[4], f = m_mat[5];

    float g = other[0], h = other[1], i = other[2];
    float j = other[3], k = other[4], l = other[5];

    res[0] = a*g + b*j;
    res[1] = a*h + b*k;
    res[2] = a*i + b*l + c;
    res[3] = d*g + e*j;
    res[4] = d*h + e*k;
    res[5] = d*i + e*l + f;

    return FedMatrix3x3(res);
}

FedMatrix3x3& FedMatrix3x3::operator=(const FedMatrix3x3& other) {
    if (this != &other) {
        for (int i = 0; i < 6; i++)
            m_mat[i] = other[i];
    }
    return *this;
}
