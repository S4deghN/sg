#ifndef SG_H
#define SG_H

#include <stdint.h>
#include <malloc.h>
#include <errno.h>
#include <stdio.h>
#include <math.h>

#define clamp_top(val, max) ((val) > (max) ? (max) : (val))

typedef unsigned int uint;

typedef struct {
    uint32_t* pixel;
    uint w;
    uint h;
} SgFrame;

void sg_frame_init(SgFrame* frame, uint w, uint h)
{
    frame->pixel = calloc(h*w, sizeof(*(frame->pixel)));
    frame->w = w;
    frame->h = h;
}

int sg_frame_save_as_ppm(const SgFrame* frame, const char* file_path)
{
    FILE* f = fopen(file_path, "w+");
    if (!f) {
        perror("Could not open file");
        goto finish;
    }

    int n = fprintf(f, "P6 %u %u 255\n", frame->w, frame->h);
    if (n < 0) {
        perror("Could write file");
        goto finish;
    }

    for (size_t i = 0; i < frame->w * frame->h; ++i) {
        fwrite(frame->pixel+i, 3, 1, f);
    }

finish:
    if (f) fclose(f);
    return errno;
}

void sg_fill_rect(SgFrame* frame, size_t x0, size_t y0, size_t w, size_t h,
    uint32_t color)
{
    uint x_end = clamp_top(x0 + w, frame->w);
    uint y_end = clamp_top(y0 + h, frame->h);

    for (uint y = y0; y < y_end; ++y) {
        for (uint x = x0; x < x_end; ++x) {
            frame->pixel[y*frame->w + x] = color;
        }
    }
}

void sg_draw_pixel_map(SgFrame* frame, uint32_t* pixel_map, size_t x0, size_t y0,
    size_t w, size_t h)
{
    uint x_end = clamp_top(x0 + w, frame->w);
    uint y_end = clamp_top(y0 + h, frame->h);

    for (uint y = y0; y < y_end; ++y) {
        for (uint x = x0; x < x_end; ++x) {
            frame->pixel[y*frame->w + x] = pixel_map[(y-y0)*w + (x-x0)];
        }
    }
}

void sg_draw_bit_map(SgFrame* frame, uint8_t* bit_map, size_t x0, size_t y0,
    size_t w, size_t h, uint32_t color)
{
    uint x_end = clamp_top(x0 + w, frame->w);
    uint y_end = clamp_top(y0 + h, frame->h);
    uint byte_w = (w + 7) / 8;
    uint8_t byte = 0;

    for (uint y = y0; y < y_end; ++y) {
        for (uint x = x0, i = 0; x < x_end; ++x, ++i) {
            if ((i & 7) == 0) byte = bit_map[(y-y0)*byte_w + i/8];
            if (byte & 0x80) frame->pixel[y*frame->w + x] = color;
            byte <<= 1;
        }
    }
}

void sg_draw_circle_point(SgFrame* frame, int x0, int y0, int i, int j, uint32_t color)
{
    const int w_max = frame->w;
    const int h_max = frame->h;
    const int index[8][2] = {
        {(y0 + j), (x0 + i)},
        {(y0 + j), (x0 - i)},
        {(y0 - j), (x0 + i)},
        {(y0 - j), (x0 - i)},
        {(y0 + i), (x0 + j)},
        {(y0 + i), (x0 - j)},
        {(y0 - i), (x0 + j)},
        {(y0 - i), (x0 - j)},
    };
    for (int n = 0; n < 8; ++n) {
        if (0 <= index[n][0] && index[n][0] < h_max &&
            0 <= index[n][1] && index[n][1] < w_max) {
            frame->pixel[index[n][0]*w_max + index[n][1]] = color;
        }
    }
}

// TODO: Rewrite.
void sg_fill_circle(SgFrame* frame, uint x0, uint y0, uint r, uint32_t color)
{
    uint x_end = clamp_top(x0 + r, frame->w);
    uint y_end = clamp_top(y0 + r, frame->h);

    int rx_excess = r > x0 ? r - x0 : 0;
    int ry_excess = r > y0 ? r - y0 : 0;

    x0 = x0 + rx_excess - r;
    y0 = y0 + ry_excess - r;

    int j = -r + ry_excess;
    for (uint y = y0; y < y_end; ++y, ++j) {
        int i = -r + rx_excess;
        for (uint x = x0; x < x_end; ++x, ++i) {
            // i^2 + j^2 = r^2 but this is not continuous math.
            uint x2y2 = (i+1)*i + (j+1)*j;
            if (x2y2 < r*r) frame->pixel[y*frame->w + x] = color;
        }
    }
}

// https://csustan.csustan.edu/~tom/Lecture-Notes/Graphics/DDA-Circle.pdf
void sg_draw_circle(SgFrame* frame, uint x0, uint y0, uint r, uint32_t color) {
    int i = 0;
    int dda_i = 1;
    int j = r;
    int dda_j = -2*r;
    int d = 1 - r;
    sg_draw_circle_point(frame, x0, y0, i, j, color);
    while (i < j) {
        if (d >= 0) {
            --j;
            dda_j += 2;
            d += dda_j;
        }
        ++i;
        dda_i += 2;
        d += dda_i;
        sg_draw_circle_point(frame, x0, y0, i, j, color);
    }
}

// TODO: Doesn't draw a perfect circle.
void sg_draw_circle2(SgFrame* frame, uint x0, uint y0, uint r, uint32_t color) {
    int i = 0;
    int j = r;
    int d = 1 - r;
    sg_draw_circle_point(frame, x0, y0, i, j, color);
    while (i < j) {
        ++i;
        if (d < 0) {
            d += 2*i + 3;
        } else {
            --j;
            d += 2*(i - j) + 5;
        }
        sg_draw_circle_point(frame, x0, y0, i, j, color);
    }
}

#endif
