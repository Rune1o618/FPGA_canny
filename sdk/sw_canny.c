#include "sw_canny.h"
#include "stdlib.h"
#include "math.h"
#include "xil_printf.h"

void Grayscale(struct pixel* in, uint8_t dst[][128])
{
	uint8_t index1=0, index2=0;
	int holder1=0, holder2=0;
	for(uint32_t i=0 ; i< 128*128 ; ++i)
	{
		holder2 = (uint32_t)in[i].blue | ((uint32_t)in[i].green << 8) | ((uint32_t)in[i].red << 16);
		holder2 = holder2 & 0x00FFFFFF;
		holder1 = 9437*(holder2 & 0x0000ff);
		holder1 += 38469*((holder2 & 0x00ff00) >> 8 );
		holder1 += 19595*((holder2 & 0xff0000) >> 16);
		holder1 >>= 16;

		if(holder1 < 0) {
			holder1 = 0;
		}
		else if(holder1 > 255) {
			holder1 = 255;
		}
//		xil_printf("%x\r\n", holder1);
		dst[index2][index1] = holder1;
//		xil_printf("%x\r\n", dst[index2][index1]);'


		if(index1 == 127)
		{
			index1 = 0;
			index2++;
		}
		else
			index1++;
	}
}

void GaussianBlur(uint8_t src[][128], uint8_t dst[][128])
{

	uint8_t line_buf[KERNEL_SIZE_GAUSS][WIDTH]; // 5 x 128 array of uint8_t (soo pixels here (gray))
	uint8_t window_buf[KERNEL_SIZE_GAUSS][KERNEL_SIZE_GAUSS]; // 5x5

	//-- 5x5 Gaussian kernel (8bit left shift)
	int GAUSS_KERNEL[KERNEL_SIZE_GAUSS][KERNEL_SIZE_GAUSS] = { {1,  4,  6,  4, 1},
								 {4, 16, 24, 16, 4},
								 {6, 24, 36, 24, 6},
								 {4, 16, 24, 16, 4},
								 {1,  4,  6,  4, 1} };

	// image proc loop
	for(int yi = 0; yi < HEIGHT; yi++) {
		for(int xi = 0; xi < WIDTH; xi++) {

			//--- gaussian bler
			int pix_gauss;

			//-- line buffer
			for(int yl = 0; yl < KERNEL_SIZE_GAUSS - 1; yl++) {
				line_buf[yl][xi] = line_buf[yl + 1][xi];
			}

			// write to line buffer
			line_buf[KERNEL_SIZE_GAUSS - 1][xi] = src[xi][yi]; // changed here

			//-- window buffer
			for(int yw = 0; yw < KERNEL_SIZE_GAUSS; yw++) {
				for(int xw = 0; xw < KERNEL_SIZE_GAUSS - 1; xw++) {
					window_buf[yw][xw] = window_buf[yw][xw + 1];
				}
			}

			// write to window buffer
			for(int yw = 0; yw < KERNEL_SIZE_GAUSS; yw++) {
				window_buf[yw][KERNEL_SIZE_GAUSS - 1] = line_buf[yw][xi];
			}

			//-- convolution
			pix_gauss = 0;
			for(int yw = 0; yw < KERNEL_SIZE_GAUSS; yw++) {
				for(int xw = 0; xw < KERNEL_SIZE_GAUSS; xw++) {
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

void Sobel(uint8_t src[][128], struct GradPix dst[][128]) {

        uint8_t line_buf[KERNEL_SIZE_SOBEL][WIDTH];
        uint8_t window_buf[KERNEL_SIZE_SOBEL][KERNEL_SIZE_SOBEL];

        //-- 3x3 Horizontal Sobel kernel
        int H_SOBEL_KERNEL[KERNEL_SIZE_SOBEL][KERNEL_SIZE_SOBEL] = {  { 1,  0, -1},
                                                                { 2,  0, -2},
                                                                { 1,  0, -1}   };
        //-- 3x3 vertical Sobel kernel
        int V_SOBEL_KERNEL[KERNEL_SIZE_SOBEL][KERNEL_SIZE_SOBEL] = {  { 1,  2,  1},
                                                                { 0,  0,  0},
                                                                {-1, -2, -1}   };


        // image proc loop
        for(int yi = 0; yi < HEIGHT; yi++) {
            for(int xi = 0; xi < WIDTH; xi++) {

                //--- sobel
                int pix_sobel;
                enum GradDir grad_sobel;

                //-- line buffer
                for(int yl = 0; yl < KERNEL_SIZE_SOBEL - 1; yl++) {
                    line_buf[yl][xi] = line_buf[yl + 1][xi];
                }
                // write to line buffer
                line_buf[KERNEL_SIZE_SOBEL - 1][xi] = src[xi][yi];

                //-- window buffer
                for(int yw = 0; yw < KERNEL_SIZE_SOBEL; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE_SOBEL - 1; xw++) {
                        window_buf[yw][xw] = window_buf[yw][xw + 1];
                    }
                }
                // write to window buffer
                for(int yw = 0; yw < KERNEL_SIZE_SOBEL; yw++) {
                    window_buf[yw][KERNEL_SIZE_SOBEL - 1] = line_buf[yw][xi];
                }

                //-- convolution
                int pix_h_sobel = 0;
                int pix_v_sobel = 0;

                // convolution using by holizonal kernel
                for(int yw = 0; yw < KERNEL_SIZE_SOBEL; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE_SOBEL; xw++) {
                        pix_h_sobel += window_buf[yw][xw] * H_SOBEL_KERNEL[yw][xw];
                    }
                }

                // convolution using by vertical kernel
                for(int yw = 0; yw < KERNEL_SIZE_SOBEL; yw++) {
                    for(int xw = 0; xw < KERNEL_SIZE_SOBEL; xw++) {
                        pix_v_sobel += window_buf[yw][xw] * V_SOBEL_KERNEL[yw][xw];
                    }
                }
                pix_sobel = sqrt((float)(pix_h_sobel * pix_h_sobel + pix_v_sobel * pix_v_sobel));

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
                if((KERNEL_SIZE_SOBEL < xi && xi < WIDTH - KERNEL_SIZE_SOBEL) &&
                   (KERNEL_SIZE_SOBEL < yi && yi < HEIGHT - KERNEL_SIZE_SOBEL)) {
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

void NonMaxSuppression(struct GradPix src[][128], uint8_t dst[][128]) {
    const int WINDOW_SIZE = 3;

    struct GradPix line_buf[WINDOW_SIZE][WIDTH];
    struct GradPix window_buf[WINDOW_SIZE][WINDOW_SIZE];

    // image proc loop
    for(int yi = 0; yi < HEIGHT; yi++) {
        for(int xi = 0; xi < WIDTH; xi++) {

            //--- non-maximum suppression
            uint8_t value_nms;
            enum GradDir grad_nms;

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


void sw_process_image(struct pixel in[], uint8_t out[][128])
{
	uint8_t img1[128][128];
	uint8_t img2[128][128];
	struct GradPix img3[128][128];

	Grayscale(in, img1);
	GaussianBlur(img1, img2);
	Sobel(img2, img3);
	NonMaxSuppression(img3, out);
}
