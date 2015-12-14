/*
 * Copyright 2015 Federico Menozzi
 */

#include <GCanvas.h>
#include <GBitmap.h>
#include <GColor.h>
#include <GRect.h>
#include <GPoint.h>
#include <GShader.h>

#include <algorithm>
#include <stack>

#include "FedUtil.h"
#include "FedMatrix3x3.h"
#include "FedVector2.h"

class FedCanvas : public GCanvas {
public:
    FedCanvas(const GBitmap& bitmap) {
        m_bitmap = bitmap;
        m_ctms   = std::stack<FedMatrix3x3>();

        m_ctms.push(FedMatrix3x3());
    }

    void clear(const GColor& color) override;
    void fillRect(const GRect& rect, const GColor& color) override;
    void fillBitmapRect(const GBitmap& src, const GRect& dst) override;
    void fillBitmapNine(const GBitmap& src, const GIRect& center, const GRect& dst) override;
    void fillConvexPolygon(const GPoint points[], int count, const GColor& color) override;

    void save() override;
    void restore() override;
    void concat(const float matrix[6]) override;

    void shadeRect(const GRect& rect, GShader* shader) override;
    void shadeConvexPolygon(const GPoint points[], int count, GShader* shader) override;

    void strokePolygon(const GPoint points[], int count, bool isClosed, const Stroke& stroke, GShader* shader) override;
private:
    GBitmap m_bitmap;
    std::stack<FedMatrix3x3> m_ctms;

    void fillPoly(const GPoint points[], int count, GShader* shader);
    FedMatrix3x3& getCTM();
};

GCanvas* GCanvas::Create(const GBitmap& bitmap) {
    if (bitmap.width() <= 0
        || bitmap.height() <= 0
        || bitmap.rowBytes() < bitmap.width() * sizeof(GPixel)
        || bitmap.pixels() == NULL)
        return NULL;
    return new FedCanvas(bitmap);
}

void FedCanvas::clear(const GColor& color) {
    GPixel srcpx = FedUtil::toPixel(color);

    int w  = m_bitmap.width();
    int h  = m_bitmap.height();
    int rb = m_bitmap.rowBytes();

    GPixel* dst = m_bitmap.pixels();

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            dst[x] = srcpx;
        dst = (GPixel*)((char*)dst + rb);
    }
}

void FedCanvas::fillRect(const GRect& rect, const GColor& color) {
    // TODO: Add non-rotated rect optimization
    if (!rect.isEmpty()) {
        GPoint pts[4];
        FedUtil::convertToQuad(rect, pts);
        fillConvexPolygon(pts, 4, color);
    }
}

void FedCanvas::fillBitmapRect(const GBitmap& src, const GRect& dst) {
    GIRect roi = GIRect::MakeWH(m_bitmap.width(), m_bitmap.height());
    if (!roi.intersect(dst.round()))
        return;

    GPoint points[4];
    FedUtil::convertToQuad(dst, points);

    GRect srcrect = GRect::MakeWH(src.width(), src.height());

    FedMatrix3x3 r2r = FedUtil::mapRectToRectMat(srcrect, dst);

    GShader* shader = GShader::FromBitmap(src, r2r.getFloats());
    fillPoly(points, 4, shader);
    delete shader;
}

void FedCanvas::fillBitmapNine(const GBitmap& src, const GIRect& center, const GRect& dst) {
    GRect src_corners_lrtb_center[9];
    FedUtil::divideIntoNine(src, center, GRect::MakeWH(src.width(), src.height()), src_corners_lrtb_center);

    GRect dst_corners_lrtb_center[9];
    FedUtil::divideIntoNine(src, center, dst, dst_corners_lrtb_center);

    GBitmap corners_lrtb_center_slices[9];
    for (int i = 0; i < 9; i++)
        corners_lrtb_center_slices[i] = FedUtil::bitmapSlice(src, src_corners_lrtb_center[i]);

    for (int i = 0; i < 9; i++)
        fillBitmapRect(corners_lrtb_center_slices[i], dst_corners_lrtb_center[i]);
}

void FedCanvas::fillPoly(const GPoint points[], int count, GShader* shader) {
    if (count <= 2)
        return;

    GPoint* xform_points = new GPoint[count];
    FedMatrix3x3 ctm = getCTM();
    ctm.apply(points, count, xform_points);

    shader->setContext(ctm.getFloats());

    int w  = m_bitmap.width();
    int h  = m_bitmap.height();
    int rb = m_bitmap.rowBytes();

    FedUtil::FedEdge* edges = new FedUtil::FedEdge[3*count];
    int num_edges = FedUtil::pointsToEdges(xform_points, count, edges, w, h);

    std::sort(edges, edges + num_edges, FedUtil::byYThenX);

    int c = 0;
    FedUtil::FedEdge left  = edges[c++];
    FedUtil::FedEdge right = edges[c++];
    int curr_y = left.top_y;

    GPixel* device = (GPixel*)((char*)(m_bitmap.pixels()) + curr_y * rb);

    GPixel* row = new GPixel[w];

    while (true) {
        while (curr_y < left.bot_y && curr_y < right.bot_y) {
            int x0 = GRoundToInt(left.curr_x);
            int x1 = GRoundToInt(right.curr_x);

            int count = x1-x0;
            if (count > 0) {
                shader->shadeRow(x0, curr_y, count, row);
                FedUtil::blendRow(row, device + x0, count);
            }

            left.curr_x  += left.m;
            right.curr_x += right.m;

            curr_y++;

            device = (GPixel*)((char*)device + rb);
        }

        if (c >= num_edges)
            break;

        if (curr_y >= left.bot_y && c != num_edges)
            left = edges[c++];
        if (curr_y >= right.bot_y && c != num_edges)
            right = edges[c++];
    }

    delete[] xform_points;
    delete[] edges;
    delete[] row;
}

FedMatrix3x3& FedCanvas::getCTM() {
    return m_ctms.top();
}

void FedCanvas::fillConvexPolygon(const GPoint points[], int count, const GColor& color) {
    GShader* shader = GShader::FromColor(color);
    fillPoly(points, count, shader);
    delete shader;
}

void FedCanvas::save() {
    m_ctms.push(getCTM());
}

void FedCanvas::restore() {
    if (!m_ctms.empty())
        m_ctms.pop();
}

void FedCanvas::concat(const float matrix[6]) {
    getCTM() = getCTM() * FedMatrix3x3(matrix);
}

void FedCanvas::shadeRect(const GRect& rect, GShader* shader) {
    if (!rect.isEmpty()) {
        GPoint pts[4];
        FedUtil::convertToQuad(rect, pts);
        shadeConvexPolygon(pts, 4, shader);
    }
}

void FedCanvas::shadeConvexPolygon(const GPoint points[], int count, GShader* shader) {
    fillPoly(points, count, shader);
}

void FedCanvas::strokePolygon(const GPoint points[], int count, bool isClosed, const Stroke& stroke, GShader* shader) {
    if ((isClosed && count <= 2) || (count <= 1))
        return;

    float mag = stroke.fWidth / 2;
    bool  cap = stroke.fAddCap;

    for (int m = 1; m <= count; m++) {
        int i = m-1;
        int j = m % count;
        int k = (m+1) % count;

        if (!isClosed && j == 0)
            break;

        GPoint A = points[i];
        GPoint B = points[j];
        GPoint C = points[k];

        FedVector2 AB(A,B);

        FedVector2 AB_scaled = AB.scaled(mag);

        FedVector2 AB_scaled_ccw = AB_scaled.rotate90ccw();
        FedVector2 AB_scaled_cw  = AB_scaled.rotate90cw();

        GPoint shell[4];
        shell[0] = A + AB_scaled_ccw;
        shell[1] = A + AB_scaled_cw;
        shell[2] = B + AB_scaled_cw;
        shell[3] = B + AB_scaled_ccw;
        if (cap && !isClosed && i == 0) {
            shell[0] = shell[0] - AB_scaled;
            shell[1] = shell[1] - AB_scaled;
        }
        if (cap && !isClosed && j == count-1) {
            shell[2] = shell[2] + AB_scaled;
            shell[3] = shell[3] + AB_scaled;
        }
        shadeConvexPolygon(shell, 4, shader);

        FedVector2 BC(B,C);
        FedVector2 BA(B,A);

        GPoint Q = shell[3];
        GPoint R = B + BC.scaled(mag).rotate90ccw();

        GPoint joint[4];
        joint[0] = Q;
        joint[1] = B;
        joint[2] = R;

        FedVector2 BQ(B,Q);
        FedVector2 BR(B,R);

        float sgn = BA.crossmag(BC);
        if (sgn > 0) {
            BQ = -BQ;
            BR = -BR;

            joint[0] = shell[2];
            joint[2] = B - BC.scaled(mag).rotate90ccw();
        }

        if (m != count-1 || isClosed) {
            float h_norm = sqrtf(2 / (1 - BA.unit().dot(BC.unit())));
            if (h_norm > stroke.fMiterLimit) {
                shadeConvexPolygon(joint, 3, shader);
            } else {
                FedVector2 BP = (BR + BQ).scaled(h_norm * mag);

                joint[3] = B + BP;

                shadeConvexPolygon(joint, 4, shader);
            }
        }
    }
}
