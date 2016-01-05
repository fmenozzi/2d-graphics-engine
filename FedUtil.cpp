/*
 * Copyright 2015 Federico Menozzi
 */

#include "FedUtil.h"

#include <GPixel.h>
#include <GColor.h>
#include <GRect.h>
#include <GBitmap.h>
#include <GPoint.h>

#include <algorithm>

namespace FedUtil {

GPixel blend(GPixel src, GPixel dst) {
    static constexpr int magic = (1<<16) | (1<<8) | 1;

    unsigned dstA = GPixel_GetA(dst), srcA = GPixel_GetA(src);
    unsigned dstR = GPixel_GetR(dst), srcR = GPixel_GetR(src);
    unsigned dstG = GPixel_GetG(dst), srcG = GPixel_GetG(src);
    unsigned dstB = GPixel_GetB(dst), srcB = GPixel_GetB(src);

    unsigned sub = 255-srcA;

    unsigned finalA = srcA + (((sub * dstA * magic) + (1<<23)) >> 24);
    unsigned finalR = srcR + (((sub * dstR * magic) + (1<<23)) >> 24);
    unsigned finalG = srcG + (((sub * dstG * magic) + (1<<23)) >> 24);
    unsigned finalB = srcB + (((sub * dstB * magic) + (1<<23)) >> 24);

    return GPixel_PackARGB(finalA, finalR, finalG, finalB);
}

GPixel toPixel(const GColor& color) {
    GColor uc = color.pinToUnit();

    float fA255 = uc.fA * 255.9999f;

    unsigned a = (unsigned)(fA255);
    unsigned r = (unsigned)(uc.fR * fA255);
    unsigned g = (unsigned)(uc.fG * fA255);
    unsigned b = (unsigned)(uc.fB * fA255);

    return GPixel_PackARGB(a, r, g, b);
}

GColor toColor(GPixel pixel) {
    float a = GPixel_GetA(pixel) / 256.f;
    float r = GPixel_GetR(pixel) / 256.f;
    float g = GPixel_GetG(pixel) / 256.f;
    float b = GPixel_GetB(pixel) / 256.f;

    return GColor::MakeARGB(a, r, g, b);
}

int numEdgesAfterClipping(GPoint p0, GPoint p1, int w) {
    int num_edges = 1;
    if (p0.x() < 0)
        num_edges++;
    if (p1.x() > w)
        num_edges++;
    return num_edges;
}

int pointsToEdges(const GPoint points[], int count, FedEdge edges[], int w, int h) {
    int c = 0;
    for (int k = 1; k <= count; k++) {
        int i = k-1;
        int j = k % count;

        // Sort in y
        GPoint p0 = points[i];
        GPoint p1 = points[j];
        p0 = points[i].y() < points[j].y() ? points[i] : points[j];
        p1 = points[i].y() > points[j].y() ? points[i] : points[j];

        // Reject above and below
        if ((p0.y() < 0 && p1.y() < 0) || (p0.y() > h && p1.y() > h))
            continue;

        // Chop as needed wrt top and bottom
        if (p0.y() < 0) {
            float p0x = p0.x() + (p1.x() - p0.x()) * ((0 - p0.y()) / (p1.y() - p0.y()));
            float p0y = 0;
            p0.set(p0x, p0y);
        }
        if (p1.y() > h) {
            float p1x = p0.x() + (p1.x() - p0.x()) * ((h - p0.y()) / (p1.y() - p0.y()));
            float p1y = h;
            p1.set(p1x, p1y);
        }

        // Sort in x
        GPoint temp0 = p0;
        GPoint temp1 = p1;
        p0 = temp0.x() <= temp1.x() ? temp0 : temp1;
        p1 = temp0.x() >  temp1.x() ? temp0 : temp1;

        // Pin as needed wrt left and right
        if ((p0.x() < 0 && p1.x() < 0) || (p0.x() > w && p1.x() > w)) {
            int top_y  = p0.y() < p1.y() ? p0.y() : p1.y();
            int bot_y  = p0.y() > p1.y() ? p0.y() : p1.y();
            int curr_x = p0.x() < 0 ? 0 : w;
            edges[c++] = FedEdge(std::max(top_y, 0), std::min(bot_y, h), 0, curr_x);
            continue;
        }

        // Project as needed
        int edges_after_clipping = numEdgesAfterClipping(p0, p1, w);
        if (edges_after_clipping == 1) {
            int top_y = GRoundToInt(p0.y() < p1.y() ? p0.y() : p1.y());
            int bot_y = GRoundToInt(p0.y() > p1.y() ? p0.y() : p1.y());
            if (top_y != bot_y) {
                float m      = (p1.x() - p0.x()) / (p1.y() - p0.y());
                float curr_x = p0.x() + m * (top_y+0.5 - p0.y());
                edges[c++]   = FedEdge(top_y, bot_y, m, curr_x);
            }
        } else if (edges_after_clipping == 2) {
            if (p0.x() < 0) {
                GPoint proj = GPoint::Make(0, p0.y());

                float clipx = 0;
                float clipy = p0.y() + (p1.y() - p0.y()) * (0 - p0.x())/(p1.x() - p0.x());
                GPoint clip = GPoint::Make(clipx, clipy);

                // Projected edge
                int top_y  = GRoundToInt(proj.y() < clip.y() ? proj.y() : clip.y());
                int bot_y  = GRoundToInt(proj.y() > clip.y() ? proj.y() : clip.y());
                edges[c++] = FedEdge(top_y, bot_y, 0, 0);

                // Clipped edge (check not horizontal)
                top_y = GRoundToInt(clip.y() < p1.y() ? clip.y() : p1.y());
                bot_y = GRoundToInt(clip.y() > p1.y() ? clip.y() : p1.y());
                if (top_y != bot_y) {
                    float m      = (clip.x() - p1.x()) / (clip.y() - p1.y());
                    float curr_x = clip.y() < p1.y() ? clip.x() : p1.x();
                    curr_x       = curr_x + m * (top_y+0.5 - (clip.y() < p1.y() ? clip.y() : p1.y()));
                    edges[c++]   = FedEdge(top_y, bot_y, m, curr_x);
                }
            } else {  // p1.x() > w
                GPoint proj = GPoint::Make(w, p1.y());

                float clipx = w;
                float clipy = p0.y() + (p1.y() - p0.y()) * (w - p0.x())/(p1.x() - p0.x());
                GPoint clip = GPoint::Make(clipx, clipy);

                // Projected edge
                int top_y  = GRoundToInt(proj.y() < clip.y() ? proj.y() : clip.y());
                int bot_y  = GRoundToInt(proj.y() > clip.y() ? proj.y() : clip.y());
                edges[c++] = FedEdge(top_y, bot_y, 0, w);

                // Clipped edge (check not horizontal)
                top_y = GRoundToInt(clip.y() < p0.y() ? clip.y() : p0.y());
                bot_y = GRoundToInt(clip.y() > p0.y() ? clip.y() : p0.y());
                if (top_y != bot_y) {
                    float m      = (clip.x() - p1.x()) / (clip.y() - p1.y());
                    float curr_x = clip.y() < p0.y() ? clip.x() : p0.x();
                    curr_x       = curr_x + m * (top_y+0.5 - (clip.y() < p0.y() ? clip.y() : p0.y()));
                    edges[c++]   = FedEdge(top_y, bot_y, m, curr_x);
                }
            }
        } else {  // edges_after_clipping == 3
            GPoint leftproj  = GPoint::Make(0, p0.y());
            GPoint rightproj = GPoint::Make(w, p1.y());

            float leftclipx = 0;
            float leftclipy = p0.y() + (p1.y() - p0.y()) * (0 - p0.x())/(p1.x() - p0.x());
            GPoint leftclip = GPoint::Make(leftclipx, leftclipy);

            float rightclipx = w;
            float rightclipy = p0.y() + (p1.y() - p0.y()) * (h - p0.x())/(p1.x() - p0.x());
            GPoint rightclip = GPoint::Make(rightclipx, rightclipy);

            // Left projected edge
            int top_y  = GRoundToInt(leftproj.y() < leftclip.y() ? leftproj.y() : leftclip.y());
            int bot_y  = GRoundToInt(leftproj.y() > leftclip.y() ? leftproj.y() : leftclip.y());
            edges[c++] = FedEdge(top_y, bot_y, 0, 0);

            // Interior edge
            top_y = GRoundToInt(leftclip.y() < rightclip.y() ? leftclip.y() : rightclip.y());
            bot_y = GRoundToInt(leftclip.y() > rightclip.y() ? leftclip.y() : rightclip.y());
            if (top_y != bot_y) {
                float m      = (leftclip.x() - rightclip.x()) / (leftclip.y() - rightclip.y());
                float curr_x = leftclip.y() < rightclip.y() ? leftclip.x() : rightclip.x();
                curr_x       = curr_x + m * (top_y+0.5 - (leftclip.y() < rightclip.y() ? leftclip.y() : rightclip.y()));
                edges[c++]   = FedEdge(top_y, bot_y, m, curr_x);
            }

            // Right projected edge
            top_y = GRoundToInt(rightproj.y() < rightclip.y() ? rightproj.y() : rightclip.y());
            bot_y = GRoundToInt(rightproj.y() > rightclip.y() ? rightproj.y() : rightclip.y());
            edges[c++] = FedEdge(top_y, bot_y, 0, w);
        }
    }
    return c;
}

bool byYThenX(FedEdge a, FedEdge b) {
    if (a.top_y == b.top_y) {
        float a_mid_x = a.curr_x + a.m/2 * (a.bot_y - a.top_y);
        float b_mid_x = b.curr_x + b.m/2 * (b.bot_y - b.top_y);
        return a_mid_x < b_mid_x;
    }
    return a.top_y < b.top_y;
}

void convertToQuad(const GRect& rect, GPoint pts[4]) {
    pts[0] = GPoint::Make(rect.left(),  rect.top());
    pts[1] = GPoint::Make(rect.left(),  rect.bottom());
    pts[2] = GPoint::Make(rect.right(), rect.bottom());
    pts[3] = GPoint::Make(rect.right(), rect.top());
}

FedMatrix3x3 mapRectToRectMat(const GRect& src, const GRect& dst) {
    float res[6];

    float t1x = -src.left();
    float t1y = -src.top();

    float sx = dst.width() / src.width();
    float sy = dst.height() / src.height();

    float t2x = dst.left();
    float t2y = dst.top();

    res[0] = sx;
    res[1] = 0;
    res[2] = t1x*sx + t2x;

    res[3] = 0;
    res[4] = sy;
    res[5] = t1y*sy + t2y;

    return FedMatrix3x3(res);
}

void blendRow(const GPixel src[], GPixel dst[], int count) {
    for (int i = 0; i < count; i++)
        dst[i] = GPixel_GetA(src[i]) == 255 ? src[i] : blend(src[i], dst[i]);
}

GColor lerp(const GColor& c0, float t, const GColor& c1) {
    GColor res;

    res.fA = c0.fA*(1-t) + c1.fA*t;
    res.fR = c0.fR*(1-t) + c1.fR*t;
    res.fG = c0.fG*(1-t) + c1.fG*t;
    res.fB = c0.fB*(1-t) + c1.fB*t;

    return res;
}

// TODO: slice should really be a GIRect
GBitmap bitmapSlice(const GBitmap& src, const GRect& slice) {
    GBitmap res;

    res.fWidth    = slice.width();
    res.fHeight   = slice.height();
    res.fRowBytes = src.rowBytes();
    res.fPixels   = src.getAddr(slice.x(), slice.y());

    return res;
}

void divideIntoNine(const GBitmap& src, const GIRect& center, const GRect& dst, GRect corners_lrtb_center[9]) {
    int dstx = GRoundToInt(dst.x());
    int dsty = GRoundToInt(dst.y());
    int dstw = GRoundToInt(dst.width());
    int dsth = GRoundToInt(dst.height());

    int crw = center.x();
    int crh = center.y();

    int cx = dstx + crw;
    int cy = dsty + crh;
    int cw = dstw - (2*crw);
    int ch = dsth - (2*crh);

    // Corners
    corners_lrtb_center[0] = GRect::MakeXYWH(dstx,            dsty,            crw, crh);
    corners_lrtb_center[1] = GRect::MakeXYWH(dstx + crw + cw, dsty,            crw, crh);
    corners_lrtb_center[2] = GRect::MakeXYWH(dstx + crw + cw, dsty + crh + ch, crw, crh);
    corners_lrtb_center[3] = GRect::MakeXYWH(dstx,            dsty + crh + ch, crw, crh);

    // LRTB
    corners_lrtb_center[4] = GRect::MakeXYWH(dstx,            dsty + crh,      crw, ch);
    corners_lrtb_center[5] = GRect::MakeXYWH(dstx + crw + cw, dsty + crh,      crw, ch);
    corners_lrtb_center[6] = GRect::MakeXYWH(dstx + crw,      dsty,            cw,  crh);
    corners_lrtb_center[7] = GRect::MakeXYWH(dstx + crw,      dsty + crh + ch, cw,  crh);

    // Center
    corners_lrtb_center[8] = GRect::MakeXYWH(cx, cy, cw, ch);
}

}  // namespace FedUtil
