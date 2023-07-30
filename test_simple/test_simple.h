#ifndef __TEST_SIMPLE_H
#define __TEST_SIMPLE_H

#include <stdint.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_math.h>

#define HEIGHT					128
#define WIDTH					128

#define DATA_SIZE				32
//#define LINE_BUFFER_LENGTH		256
#define NUM_PACKETS				2048


// definition of gradient direction
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

typedef ap_axis<32,1,1,1> data_t;

void stream_in(hls::stream<data_t> &in, uint8_t out[][128]);
void stream_out(uint8_t in[][128], hls::stream<data_t> &out);

void test_simple(hls::stream<data_t> &INPUT_STREAM, hls::stream<data_t> &OUTPUT_STREAM);

#endif
