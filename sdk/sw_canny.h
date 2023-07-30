/*
 * sw_canny.h
 *
 *  Created on: 07.01.2023
 *      Author: wszcz
 */

#ifndef SRC_SW_CANNY_H_
#define SRC_SW_CANNY_H_

#include "inttypes.h"
#include "common.h"

#define WIDTH					128
#define HEIGHT					128
#define KERNEL_SIZE_GAUSS		5
#define KERNEL_SIZE_SOBEL		3


enum GradDir {
	DIR_0,
	DIR_45,
	DIR_90,
	DIR_135
};

struct GradPix {
	uint8_t value;
	enum GradDir grad;
    };

void sw_process_image(struct pixel in[], uint8_t out[][128]);

#endif /* SRC_SW_CANNY_H_ */
