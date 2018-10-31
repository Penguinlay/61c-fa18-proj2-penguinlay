/*
 * Project 2: Performance Optimization
 */

#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#endif

#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "calc_depth_optimized.h"
#include "utils.h"

// Absolute Displacement
float ad(int dx, int dy, int md) {
    return sqrt(dx * dx + dy * dy);
}

// Squared Euclidean Distance (SED)
float sed(float a, float b) {
    if (a >= b) return (a - b) * (a - b);
    return (b - a) * (b - a);
}

// 2D -> 1D
int get1D(int x, int y, int w) {
    return w * y + x;
}

// 1D -> x Coordinate of 2D
int get2Dx(int i, int w) {
    return i % w;
}

// 1D -> y Coordinate of 2D
int get2Dy(int i, int w) {
    return i / w;
}

// Depth Map
// Input: Depth Map, Left Image, Right Image, Width, Height, Feature Width, Feature Height, Maximum Displacement
void calc_depth_optimized(float *dm, float *l, float *r, int w, int h, int fw, int fh, int md) {
    int i;                              // Current Pixel's 1D Coordinate
    // Special Case: Maximum Displacement Zero
    if (md == 0) {
        for (i = 0; i < w * h; i++) dm[i] = 0;
        return;
    }
    int x, y;                           // Current Pixel's 2D Coordinate (x, y)
    int rfx, rfy;                       // Current Right Pixel Coordinates
    float minSED, curSED;               // Minimum and Current SED Values
    float minND, curND;                 // Minimum and Current ND as Tiebreaker
    // For each pixel of the left image, ...
    // Note: Here, using 1D coordinate.
    for (i = 0; i < w * h; i++) {
        x = get2Dx(i, w);
        y = get2Dy(i, w);
        // Special Case: Left Image Feature Out of Bound
        if (x - fw < 0 || y - fh < 0 || x + fw >= w || y + fh >= h) {
            dm[i] = 0;
            continue;
        }
        // Initialize minimum SED to maximum possible SED.
        minSED = UINT_MAX;
        // Initialize minimum ND to maximum possible bitmap pixel value.
        minND = FLT_MAX;
        // For each potential right image feature, ...
        // Note: Here, using 2D coordinate of feature center's pixel.
        for (rfx = x - md; rfx <= x + md; rfx++) {
            // Special Case: Right Image Feature Out of Bound Horizontally
            if (rfx - fw < 0 || rfx + fw >= w) continue;
            for (rfy = y - md; rfy <= y + md; rfy++) {
                // Special Case: Right Image Feature Out of Bound Vertically
                if (rfy - fh < 0 || rfy + fh >= h) continue;
                // If the whole feature is not out of bound, safe to proceed.
                // Initialize current SED accumulator with zero.
                curSED = 0;
                // For each pixel in current features, ...
                // Note: Here, using 2D coordinate offset from center pixel.
                int ho, vo;             // Horizontal and Vertical Offset
                for (ho = (-1) * fw; ho <= fw; ho++) {
                    for (vo = (-1) * fh; vo <= fh; vo++) {
                        // Find total SED of current feature.
                        curSED += sed(l[get1D(x + ho, y + vo, w)], r[get1D(rfx + ho, rfy + vo, w)]);
                    }
                }
                curND = ad(rfx - x, rfy - y, md);
                // If current SED is smaller than previous minimum SED, ...
                if (curSED < minSED) {
                    minSED = curSED;
                    minND = curND;
                } else if (curSED == minSED && curND < minND) {
                    minND = curND;
                }
            }
        }
        // Save the final normalized displacement in depth map.
        dm[i] = minND;
    }
}
