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

#include "calc_depth_naive.h"
#include "calc_depth_optimized.h"
#include "utils.h"

// My code didn't pass the given test. Going back to given code and gonna work from there.

// Displacement Square
float ads(int dx, int dy) {
    return dx * dx + dy * dy;
}

float ad(int dx, int dy) {
    return sqrt(dx * dx + dy * dy);
}

float sed(float a, float b) {
    int diff = a - b;
    return diff * diff;
}

void calc_depth_optimized(float *dm, float *l, float *r, int w, int h, int fw, int fh, int md) {
    // For each pixel,
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            // If feature in left image is out of bound,
            if (y - fh < 0 || y + fh >= h || x - fw < 0 || x + fw >= w) {
                dm[w * y + x] = 0;
                continue;
            }
            float min_diff = -1;
            int min_dy = 0;
            int min_dx = 0;
            // For each feature to check in right image with respect to current pixel,
            for (int dy = -md; dy <= md; dy++) {
                for (int dx = -md; dx <= md; dx++) {
                    if (y + dy - fh < 0 || y + dy + fh >= h || x + dx - fw < 0 || x + dx + fw >= w) {
                        continue;
                    }
                    float squared_diff = 0;
                    // For each pixel in the selected feature in right image,
                    for (int box_y = -fh; box_y <= fh; box_y++) {
                        for (int box_x = -fw; box_x <= fw; box_x++) {
                            int left_x = x + box_x;
                            int left_y = y + box_y;
                            int right_x = x + dx + box_x;
                            int right_y = y + dy + box_y;
                            squared_diff += sed(l[left_y * w + left_x], r[right_y * w + right_x]);
                        }
                    }
                    if (min_diff == -1 || squared_diff < min_diff || (squared_diff == min_diff && ads(dx, dy) < ads(min_dx, min_dy))) {
                        min_diff = squared_diff;
                        min_dx = dx;
                        min_dy = dy;
                    }
                }
            }
            if (min_diff != -1) {
                if (md == 0) {
                    dm[w * y + x] = 0;
                } else {
                    dm[w * y + x] = ad(min_dx, min_dy);
                }
            } else {
                dm[w * y + x] = 0;
            }
        }
    }
}
