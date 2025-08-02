#include "sg.h"

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BATT_GRAPH_WIDTH 24
#define BATT_GRAPH_HEIGHT 11
uint8_t batt_graph[BATT_GRAPH_WIDTH * BATT_GRAPH_HEIGHT] = {
  0b01111111,0b11111111,0b11100000,
  0b11111111,0b11111111,0b11100000,
  0b10000000,0b00000000,0b00110000,
  0b10000000,0b00000000,0b00111100,
  0b10000000,0b00000000,0b00111100,
  0b10000000,0b00000000,0b00111100,
  0b10000000,0b00000000,0b00111100,
  0b10000000,0b00000000,0b00111100,
  0b10000000,0b00000000,0b00110000,
  0b11111111,0b11111111,0b11100000,
  0b01111111,0b11111111,0b11100000
};

uint32_t pixel_map[] = {
    0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0xFF,
    0xFF,0x00,0x00,0x00,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,
};

#define PLUGEDIN_GRAPH_WIDTH 8
#define PLUGEDIN_GRAPH_HEIGHT 11
uint8_t plugedin_graph[PLUGEDIN_GRAPH_WIDTH * PLUGEDIN_GRAPH_HEIGHT] = {
    0b00001111,
    0b00011110,
    0b00111100,
    0b01111000,
    0b11111111,
    0b00001110,
    0b00011100,
    0b00011000,
    0b00110000,
    0b01100000,
    0b01000000,
};

int main()
{
    const uint frame_size = 4;
    const uint width = 320*frame_size;
    const uint height = 240*frame_size;
    SgFrame frame;
    sg_frame_init(&frame, width, height);

    uint step = 40;
    uint r = step/2-1;
    for (uint j = r; j < height-r; j += step) {
        for (uint i = r; i < width-r; i += step) {
            sg_fill_circle(&frame, i, j, r-i/150-j/150, 0xff);
        }
    }

    sg_frame_save_as_ppm(&frame, "output.ppm");
}
