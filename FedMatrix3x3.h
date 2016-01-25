/*
 * Copyright 2015 Federico Menozzi
 */

#ifndef FEDMATRIX3X3_H_
#define FEDMATRIX3X3_H_

#include <GPoint.h>

class FedMatrix3x3 {
public:
    FedMatrix3x3() {
        m_mat[0] = 1;
        m_mat[1] = 0;
        m_mat[2] = 0;
        m_mat[3] = 0;
        m_mat[4] = 1;
        m_mat[5] = 0;
    }
    explicit FedMatrix3x3(const float mat[6]) {
        for (int i = 0; i < 6; i++)
            m_mat[i] = mat[i];
    }

    GPoint apply(const GPoint& point) const;
    void   apply(const GPoint points[], int count, GPoint xform_points[]) const;

    FedMatrix3x3 inv() const;

    FedMatrix3x3  operator*(const FedMatrix3x3& other) const;
    FedMatrix3x3& operator=(const FedMatrix3x3& other);
    float operator[](int i) const {
        return m_mat[i];
    }

    float* getFloats() {
        return m_mat;
    }

private:
    float m_mat[6];
};

#endif  // FEDMATRIX3X3_H_
