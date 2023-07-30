#include "test_simple.h"


//	 Y = B*0.144 + G*0.587 + R*0.299
//			pix_gray = 9437*(axis_reader.data & 0x0000ff)
//				+ 38469*((axis_reader.data & 0x00ff00) >> 8 )
//				+ 19595*((axis_reader.data & 0xff0000) >> 16);

void stream_in(hls::stream<data_t> &in, uint8_t out[][128])
{
#pragma HLS INLINE off

	data_t in_buffer;
	int holder;
	uint32_t in_count = 0;

	stream_in_i:for(uint8_t i=0; i<128; ++i)
	{
		stream_in_j:for(uint8_t j=0 ; j<4; ++j) // one line WIDTH/HEIGHT (128pix)
		{
			do
			{
#pragma HLS PIPELINE
				in_buffer = in.read();

				holder =9437*(in_buffer.data & 0x0000ff)
							+ 38469*((in_buffer.data & 0x00ff00) >> 8 )
							+ 19595*((in_buffer.data & 0xff0000) >> 16);
				holder >>= 16;
				if(holder < 0) {
					holder = 0;
				}
				else if(holder > 255) {
					holder = 255;
				}
				out[i][in_count + (j*DATA_SIZE)] = holder;
				in_count++;
			}while(!in_buffer.last);

			in_count = 0;
		}
	}
}



void stream_out(uint8_t in[][128], hls::stream<data_t> &out)
{
#pragma HLS INLINE off
	data_t out_buffer;
	unsigned int holder;
	out_buffer.keep = 0b1111;
	uint32_t out_counter = 0;
	for(uint8_t i=0; i<128; ++i)
	{
		for(uint8_t j=0; j<128; ++j)
		{
			holder = in[i][j];
			out_buffer.data = holder << 16 | holder << 8 | holder;

			if(out_counter == DATA_SIZE - 1)
			{
				out_counter = 0;
				out_buffer.last = 1;
			}
			else
			{
				out_counter++;
				out_buffer.last = 0;
			}
			out.write(out_buffer);
		}
	}

}

inline void GaussianBlur(uint8_t src[][128], uint8_t dst[][128])
{
	const int KERNEL_SIZE = 5;

	uint8_t line_buf[KERNEL_SIZE][WIDTH]; // 5 x 128 array of uint8_t (soo pixels here (gray))
	uint8_t window_buf[KERNEL_SIZE][KERNEL_SIZE]; // 5x5

	#pragma HLS ARRAY_RESHAPE variable=line_buf complete dim=1
	#pragma HLS ARRAY_PARTITION variable=window_buf complete dim=0

	//-- 5x5 Gaussian kernel (8bit left shift)
	const int GAUSS_KERNEL[KERNEL_SIZE][KERNEL_SIZE] = { {1,  4,  6,  4, 1},
														 {4, 16, 24, 16, 4},
														 {6, 24, 36, 24, 6},
														 {4, 16, 24, 16, 4},
														 {1,  4,  6,  4, 1} };

	#pragma HLS ARRAY_PARTITION variable=GAUSS_KERNEL complete dim=0

	// image proc loop
	for(int yi = 0; yi < HEIGHT; yi++) {
		for(int xi = 0; xi < WIDTH; xi++) {
			#pragma HLS PIPELINE II=1
			#pragma HLS LOOP_FLATTEN off

			//--- gaussian bler
			int pix_gauss;

			//-- line buffer
			for(int yl = 0; yl < KERNEL_SIZE - 1; yl++) {
				line_buf[yl][xi] = line_buf[yl + 1][xi];
			}

			// write to line buffer
			line_buf[KERNEL_SIZE - 1][xi] = src[xi][yi]; // changed here

			//-- window buffer
			for(int yw = 0; yw < KERNEL_SIZE; yw++) {
				for(int xw = 0; xw < KERNEL_SIZE - 1; xw++) {
					window_buf[yw][xw] = window_buf[yw][xw + 1];
				}
			}

			// write to window buffer
			for(int yw = 0; yw < KERNEL_SIZE; yw++) {
				window_buf[yw][KERNEL_SIZE - 1] = line_buf[yw][xi];
			}

			//-- convolution
			pix_gauss = 0;
			for(int yw = 0; yw < KERNEL_SIZE; yw++) {
				for(int xw = 0; xw < KERNEL_SIZE; xw++) {
					pix_gauss += window_buf[yw][xw] * GAUSS_KERNEL[yw][xw];
				}
			}

			// 8bit right shift
			pix_gauss >>= 8;

			// output
			dst[xi][yi] = pix_gauss;
		}
	}
}

inline void Sobel(uint8_t src[][128], struct GradPix dst[][128]) {
        const int KERNEL_SIZE = 3;

        uint8_t line_buf[KERNEL_SIZE][WIDTH];
        uint8_t window_buf[KERNEL_SIZE][KERNEL_SIZE];

        #pragma HLS ARRAY_RESHAPE variable=line_buf complete dim=1
        #pragma HLS ARRAY_PARTITION variable=window_buf complete dim=0

        //-- 3x3 Horizontal Sobel kernel
        const int H_SOBEL_KERNEL[KERNEL_SIZE][KERNEL_SIZE] = {  { 1,  0, -1},
                                                                { 2,  0, -2},
                                                                { 1,  0, -1}   };
        //-- 3x3 vertical Sobel kernel
        const int V_SOBEL_KERNEL[KERNEL_SIZE][KERNEL_SIZE] = {  { 1,  2,  1},
                                                                { 0,  0,  0},
                                                                {-1, -2, -1}   };

        #pragma HLS ARRAY_PARTITION variable=H_SOBEL_KERNEL complete dim=0
        #pragma HLS ARRAY_PARTITION variable=V_SOBEL_KERNEL complete dim=0

        // image proc loop
        for(int yi = 0; yi < HEIGHT; yi++) {
            for(int xi = 0; xi < WIDTH; xi++) {
                #pragma HLS PIPELINE II=1
                #pragma HLS LOOP_FLATTEN off

                //--- sobel
                int pix_sobel;
                GradDir grad_sobel;

                //-- line buffer
                for(int yl = 0; yl < KERNEL_SIZE - 1; yl++) {
                    line_buf[yl][xi] = line_buf[yl + 1][xi];
                }
                // write to line buffer
                line_buf[KERNEL_SIZE - 1][xi] = src[xi][yi];

                //-- window buffer
                for(int yw = 0; yw < KERNEL_SIZE; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE - 1; xw++) {
                        window_buf[yw][xw] = window_buf[yw][xw + 1];
                    }
                }
                // write to window buffer
                for(int yw = 0; yw < KERNEL_SIZE; yw++) {
                    window_buf[yw][KERNEL_SIZE - 1] = line_buf[yw][xi];
                }

                //-- convolution
                int pix_h_sobel = 0;
                int pix_v_sobel = 0;

                // convolution using by holizonal kernel
                for(int yw = 0; yw < KERNEL_SIZE; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE; xw++) {
                        pix_h_sobel += window_buf[yw][xw] * H_SOBEL_KERNEL[yw][xw];
                    }
                }

                // convolution using by vertical kernel
                for(int yw = 0; yw < KERNEL_SIZE; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE; xw++) {
                        pix_v_sobel += window_buf[yw][xw] * V_SOBEL_KERNEL[yw][xw];
                    }
                }

                pix_sobel = hls::sqrt(float(pix_h_sobel * pix_h_sobel + pix_v_sobel * pix_v_sobel));

                // to consider saturation
                if(255 < pix_sobel) {
                    pix_sobel = 255;
                }

                // evaluate gradient direction
                int t_int;
                if(pix_h_sobel != 0) {
                    t_int = pix_v_sobel * 256 / pix_h_sobel;
                }
                else {
                    t_int = 0x7FFFFFFF;
                }

                // 112.5° ~ 157.5° (tan 112.5° ~= -2.4142, tan 157.5° ~= -0.4142)
                if(-618 < t_int && t_int <= -106) {
                    grad_sobel = DIR_135;
                }
                // -22.5° ~ 22.5° (tan -22.5° ~= -0.4142, tan 22.5° = 0.4142)
                else if(-106 < t_int && t_int <= 106) {
                    grad_sobel = DIR_0;
                }
                // 22.5° ~ 67.5° (tan 22.5° ~= 0.4142, tan 67.5° = 2.4142)
                else if(106 < t_int && t_int < 618) {
                    grad_sobel = DIR_45;
                }
                // 67.5° ~ 112.5° (to inf)
                else {
                    grad_sobel = DIR_90;
                }

                // output
                if((KERNEL_SIZE < xi && xi < WIDTH - KERNEL_SIZE) &&
                   (KERNEL_SIZE < yi && yi < HEIGHT - KERNEL_SIZE)) {
                    dst[xi][yi].value = pix_sobel;
                    dst[xi][yi].grad  = grad_sobel;
                }
                else {
                    dst[xi][yi].value = 0;
                    dst[xi][yi].grad  = grad_sobel;
                }
            }
        }
    }

inline void NonMaxSuppression(struct GradPix src[][128], uint8_t dst[][128]) {
    const int WINDOW_SIZE = 3;

    GradPix line_buf[WINDOW_SIZE][WIDTH];
    GradPix window_buf[WINDOW_SIZE][WINDOW_SIZE];

    #pragma HLS ARRAY_RESHAPE variable=line_buf complete dim=1
    #pragma HLS ARRAY_PARTITION variable=window_buf complete dim=0

    // image proc loop
    for(int yi = 0; yi < HEIGHT; yi++) {
        for(int xi = 0; xi < WIDTH; xi++) {
            #pragma HLS PIPELINE II=1
            #pragma HLS LOOP_FLATTEN off

            //--- non-maximum suppression
            uint8_t value_nms;
            GradDir grad_nms;

            //-- line buffer
            for(int yl = 0; yl < WINDOW_SIZE - 1; yl++) {
                line_buf[yl][xi] = line_buf[yl + 1][xi];
            }
            // write to line buffer
            line_buf[WINDOW_SIZE - 1][xi] = src[xi][yi];

            //-- window buffer
            for(int yw = 0; yw < WINDOW_SIZE; yw++) {
                for(int xw = 0; xw < WINDOW_SIZE - 1; xw++) {
                    window_buf[yw][xw] = window_buf[yw][xw + 1];
                }
            }
            // write to window buffer
            for(int yw = 0; yw < WINDOW_SIZE; yw++) {
                window_buf[yw][WINDOW_SIZE - 1] = line_buf[yw][xi];
            }

            value_nms = window_buf[WINDOW_SIZE / 2][WINDOW_SIZE / 2].value;
            grad_nms = window_buf[WINDOW_SIZE / 2][WINDOW_SIZE / 2].grad;
            // grad 0° -> left, right
            if(grad_nms == DIR_0) {
                if(value_nms < window_buf[WINDOW_SIZE / 2][0].value ||
                   value_nms < window_buf[WINDOW_SIZE / 2][WINDOW_SIZE - 1].value) {
                    value_nms = 0;
                }
            }
            // grad 45° -> upper left, bottom right
            else if(grad_nms == DIR_45) {
                if(value_nms < window_buf[0][0].value ||
                   value_nms < window_buf[WINDOW_SIZE - 1][WINDOW_SIZE - 1].value) {
                    value_nms = 0;
                }
            }
            // grad 90° -> upper, bottom
            else if(grad_nms == DIR_90) {
                if(value_nms < window_buf[0][WINDOW_SIZE - 1].value ||
                   value_nms < window_buf[WINDOW_SIZE - 1][WINDOW_SIZE / 2].value) {
                    value_nms = 0;
                }
            }
            // grad 135° -> bottom left, upper right
            else if(grad_nms == DIR_135) {
                if(value_nms < window_buf[WINDOW_SIZE - 1][0].value ||
                   value_nms < window_buf[0][WINDOW_SIZE - 1].value) {
                    value_nms = 0;
                }
            }

            // output
            if((WINDOW_SIZE < xi && xi < WIDTH - WINDOW_SIZE) &&
               (WINDOW_SIZE < yi && yi < HEIGHT - WINDOW_SIZE)) {
                dst[xi][yi] = value_nms;
            }
            else {
                dst[xi][yi] = 0;
            }
        }
    }
}


void test_simple(hls::stream<data_t> &INPUT_STREAM, hls::stream<data_t> &OUTPUT_STREAM)
{
#pragma HLS INTERFACE ap_ctrl_none port=return
#pragma HLS DATAFLOW
#pragma HLS INTERFACE axis both register port=INPUT_STREAM
#pragma HLS INTERFACE axis both register port=OUTPUT_STREAM

	uint8_t img1[HEIGHT][WIDTH];
	uint8_t img2[HEIGHT][WIDTH];
	struct GradPix img3[HEIGHT][WIDTH];
	uint8_t img4[HEIGHT][WIDTH];
	// mine
	stream_in(INPUT_STREAM, img1);
	GaussianBlur(img1, img2);
	Sobel(img2, img3);
	NonMaxSuppression(img3, img4);
	stream_out(img4, OUTPUT_STREAM);
}


