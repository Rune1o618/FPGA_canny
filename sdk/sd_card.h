#ifndef __SD_CARD_H
#define __SD_CARD_H

#include "stdio.h"
#include "xsdps.h"
#include "ff.h"
#include "common.h"

#define IMG_WIDTH	128
#define IMG_HEIGHT	128

struct bmp_header{
    uint16_t type;              // "BM" (0x42, 0x4D)
    uint32_t size;              // file size
    uint16_t reserved1;         // not used (0)
    uint16_t reserved2;         // not used (0)
    uint32_t offset;            // offset to image data (54B)
    uint32_t dib_size;          // DIB header size (40B)
    uint32_t width;             // width in pixels
    uint32_t height;            // height in pixels
    uint16_t planes;            // 1
    uint16_t bpp;               // bits per pixel (24)
    uint32_t compression;       // compression type (0/1/2) 0
    uint32_t image_size;        // size of picture in bytes, 0
    uint32_t x_ppm;             // X Pixels per meter (0)
    uint32_t y_ppm;             // X Pixels per meter (0)
    uint32_t num_colors;        // number of colors (0)
    uint32_t important_colors;  // important colors (0)
} __attribute__((__packed__));

int sd_mount(int remount);
int sd_open(char* name, BYTE mode);
int sd_read(void *buffer, int size, int fromStart);
void sd_read_bmp_data(struct bmp_header* header, struct pixel* img_arr);
int sd_read_bmp_header(struct bmp_header* header);
int sd_write_bmp_header(struct bmp_header* header);
int sd_write_bmp_packet(u8* packet, u32 size);
int sd_write(const void* buffer, int size, int append);
int sd_close();

void sd_bmp_test(struct pixel* IMG);
#endif
