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
    FILE* f;

    f = fopen(file_path, "w+");
    if (!f) {
        perror("Could not open file");
        goto finish;
    }

    fprintf(f, "P6 %u %u 255\n", frame->w, frame->h)?: ({goto finish;});


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

    for (uint j = y0; j < y_end; ++j) {
        for (uint i = x0; i < x_end; ++i) {
            frame->pixel[j*frame->w + i] = color;
        }
    }
}

void sg_draw_pixel_map(SgFrame* frame, uint32_t* pixel_map, size_t x0, size_t y0,
    size_t w, size_t h)
{
    uint x_end = clamp_top(x0 + w, frame->w);
    uint y_end = clamp_top(y0 + h, frame->h);

    for (uint j = y0; j < y_end; ++j) {
        for (uint i = x0; i < x_end; ++i) {
            frame->pixel[j*frame->w + i] = pixel_map[(j-y0)*w + (i-x0)];
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

    for (uint j = y0; j < y_end; ++j) {
        for (uint i = x0, map_i = 0; i < x_end; ++i, ++map_i) {
            if ((map_i & 7) == 0) byte = bit_map[(j-y0)*byte_w + map_i/8];
            if (byte & 0x80) frame->pixel[j*frame->w + i] = color;
            byte <<= 1;
        }
    }
}

void sg_fill_circle(SgFrame* frame, uint x0, uint y0, uint r, uint32_t color)
{
    uint x_end = clamp_top(x0 + r, frame->w);
    uint y_end = clamp_top(y0 + r, frame->h);

    int xr = r < x0 ? -r : -x0;
    int yr = r < y0 ? -r : -y0;

    x0 = r < x0 ? x0 - r : 0;
    y0 = r < y0 ? y0 - r : 0;

    int y = yr;
    for (uint j = y0; j < y_end; ++j, ++y) {
        int x = xr;
        for (uint i = x0; i < x_end; ++i, ++x) {
            // x^2 + y^2 = r^2 but this is not continuous math; hence the +1.
            uint x2y2 = (x+1)*x + (y+1)*y;
            if (x2y2 < r*r)
                frame->pixel[j*frame->w + i] = color;
        }
    }
}

void sg_draw_circle() {
}

#endif
