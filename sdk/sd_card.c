#include "sd_card.h"
#include "stdlib.h"

static FATFS m_fatfs;
int m_mounted = 0;

TCHAR fileName[256];
TCHAR pathName[256];
unsigned int NumBytesWritten;
unsigned int NumBytesRead;
FIL m_fil;

int sd_mount(int remount)
{
	FRESULT Res;
	if (!m_mounted || remount) {
		Res = f_mount(&m_fatfs, pathName, 0);

		if (Res != FR_OK) {
			return XST_FAILURE;
		}
		m_mounted = 1;
	}
	return XST_SUCCESS;
}

int sd_open(char* name, BYTE mode)
{
	FRESULT Res;
	if (!m_mounted) {
		return XST_FAILURE;
	}
	strcpy(fileName, name);

	FRESULT derp;
	derp = f_open(&m_fil, fileName, mode);
	xil_printf("DEBUG || FRESULT OPEN %s: %d\r\n", fileName, derp);
	if (derp) {
		return XST_FAILURE;
	}
	Res = f_lseek(&m_fil, 0);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


int sd_read(void *buffer, int size, int fromStart)
{
	FRESULT Res;
	if (fromStart) {
		// Pointer to beginning of file .
		Res = f_lseek(&m_fil, 0);
		if (Res) {
			return XST_FAILURE;
		}
	}
	Res = f_read(&m_fil, (void*)buffer, size,
				 &NumBytesRead);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}

int sd_read_bmp_header(struct bmp_header* header)
{
	// Pointer to beginning of file .
	FRESULT Res;
	Res = f_lseek(&m_fil, 0);
	if (Res) {
		return 1;
	}

	Res = f_read(&m_fil, (void*)(&(header->type)), 2,
				 &NumBytesRead);
	xil_printf("DEBUG || Header | type: %u, bytes read: %u\r\n", header->type, NumBytesRead);
	if (Res) {
		return 2;
	}
	Res = f_read(&m_fil, (void*)(&(header->size)), 4,
				 &NumBytesRead);
	xil_printf("DEBUG || Header | size: %u, bytes read: %u\r\n", header->size, NumBytesRead);
	if (Res) {
		return 3;
	}
	Res = f_read(&m_fil, (void*)(&(header->reserved1)), 2,
				 &NumBytesRead);
	if (Res) {
		return 4;
	}
	Res = f_read(&m_fil, (void*)(&(header->reserved2)), 2,
				 &NumBytesRead);
	if (Res) {
		return 5;
	}
	Res = f_read(&m_fil, (void*)(&(header->offset)), 4,
				 &NumBytesRead);
	xil_printf("DEBUG || Header | offset: %u, bytes read: %u\r\n", header->offset, NumBytesRead);
	if (Res) {
		return 6;
	}
	Res = f_read(&m_fil, (void*)(&(header->dib_size)), 4,
				 &NumBytesRead);
	if (Res) {
		return 7;
	}
	Res = f_read(&m_fil, (void*)(&(header->width)), 4,
				 &NumBytesRead);
	xil_printf("DEBUG || Header | width: %u, bytes read: %u\r\n", header->width, NumBytesRead);
	if (Res) {
		return 8;
	}
	Res = f_read(&m_fil, (void*)(&(header->height)), 4,
				 &NumBytesRead);
	xil_printf("DEBUG || Header | height: %u, bytes read: %u\r\n", header->height, NumBytesRead);
	if (Res) {
		return 9;
	}
	Res = f_read(&m_fil, (void*)(&(header->planes)), 2,
				 &NumBytesRead);
	if (Res) {
		return 10;
	}
	Res = f_read(&m_fil, (void*)(&(header->bpp)), 2,
				 &NumBytesRead);
	if (Res) {
		return 11;
	}
	Res = f_read(&m_fil, (void*)(&(header->compression)), 4,
				 &NumBytesRead);
	if (Res) {
		return 12;
	}
	Res = f_read(&m_fil, (void*)(&(header->image_size)), 4,
				 &NumBytesRead);
	if (Res) {
		return 13;
	}
	Res = f_read(&m_fil, (void*)(&(header->x_ppm)), 4,
				 &NumBytesRead);
	if (Res) {
		return 14;
	}
	Res = f_read(&m_fil, (void*)(&(header->y_ppm)), 4,
				 &NumBytesRead);
	if (Res) {
		return 15;
	}
	Res = f_read(&m_fil, (void*)(&(header->num_colors)), 4,
				 &NumBytesRead);
	if (Res) {
		return 16;
	}
	Res = f_read(&m_fil, (void*)(&(header->important_colors)), 4,
				 &NumBytesRead);
	if (Res) {
		return 17;
	}
	return 0;
}


void sd_read_bmp_data(struct bmp_header* header, struct pixel* img_arr){
//  if(&m_fil == NULL || header == NULL){
//	  xil_printf("Stream or header problem\r\n");
//    return;
//  }
  // w == 1 && p == 1;   w == 2 && p == 2;   w == 3 && p == 3;   w == 4 && p == 0
  int padding = (4-((3*header->width)%4))%4;
  int num_of_pixels = header->width * header->height;
  unsigned int num_of_bytes_read = 0;
  unsigned int holder = 0;
//  struct pixel* Pixel[num_of_pixels];
  uint8_t burn;
  f_lseek(&m_fil, 54); //move 54B (header size)
  uint16_t index_p = 0;
  for(uint8_t i = 0; i < header->height; i++){
    for(uint8_t j = 0; j < header->width; j++){
//      Pixel[index_p] = malloc(sizeof(struct pixel));
      f_read(&m_fil, (void*)(&(img_arr[index_p].blue)), 1, &holder);
      num_of_bytes_read += holder;
      f_read(&m_fil, (void*)(&(img_arr[index_p].green)), 1, &holder);
      num_of_bytes_read += holder;
      f_read(&m_fil, (void*)(&(img_arr[index_p].red)), 1, &holder);
      num_of_bytes_read += holder;
      f_read(&m_fil, (void*)(&burn), 1, &holder); //additional byte (lenka.bmp)
      index_p++;
//      xil_printf("DEBUG || IMG | index_p: %u, BGR: %u %u %u\r\n", index_p, img_arr[index_p].blue, img_arr[index_p].green, img_arr[index_p].red);
    }
    f_lseek(&m_fil, f_tell(&m_fil) + padding);
  }
	xil_printf("DEBUG || IMG SRC | BGR: %u %u %u\r\n", img_arr[782].blue, img_arr[782].green, img_arr[782].red);
	xil_printf("DEBUG || IMG SRC | BGR: %u %u %u\r\n", img_arr[10].blue, img_arr[10].green, img_arr[10].red);
	xil_printf("DEBUG || IMG SRC | BGR: %u %u %u\r\n", img_arr[11111].blue, img_arr[11111].green, img_arr[11111].red);
//  return *Pixel;
}

int sd_write_bmp_header(struct bmp_header* header)
{
	FRESULT Res;
	Res = f_lseek(&m_fil, 0);
	if (Res) {
		return 1;
	}

	Res = f_write(&m_fil, (void*)(&(header->type)), 2,
				 &NumBytesWritten);
	if (Res) {
		return 2;
	}
	Res = f_write(&m_fil, (void*)(&(header->size)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 3;
	}
	Res = f_write(&m_fil, (void*)(&(header->reserved1)), 2,
				 &NumBytesWritten);
	if (Res) {
		return 4;
	}
	Res = f_write(&m_fil, (void*)(&(header->reserved2)), 2,
				 &NumBytesWritten);
	if (Res) {
		return 5;
	}
	Res = f_write(&m_fil, (void*)(&(header->offset)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 6;
	}
	Res = f_write(&m_fil, (void*)(&(header->dib_size)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 7;
	}
	Res = f_write(&m_fil, (void*)(&(header->width)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 8;
	}
	Res = f_write(&m_fil, (void*)(&(header->height)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 9;
	}
	Res = f_write(&m_fil, (void*)(&(header->planes)), 2,
				 &NumBytesWritten);
	if (Res) {
		return 10;
	}
	Res = f_write(&m_fil, (void*)(&(header->bpp)), 2,
				 &NumBytesWritten);
	if (Res) {
		return 11;
	}
	Res = f_write(&m_fil, (void*)(&(header->compression)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 12;
	}
	Res = f_write(&m_fil, (void*)(&(header->image_size)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 13;
	}
	Res = f_write(&m_fil, (void*)(&(header->x_ppm)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 14;
	}
	Res = f_write(&m_fil, (void*)(&(header->y_ppm)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 15;
	}
	Res = f_write(&m_fil, (void*)(&(header->num_colors)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 16;
	}
	Res = f_write(&m_fil, (void*)(&(header->important_colors)), 4,
				 &NumBytesWritten);
	if (Res) {
		return 17;
	}

//	uint8_t burn = 0xFF;
//	for(uint16_t i=0; i<IMG_HEIGHT*IMG_WIDTH; i=i+4)
//	{
//		Res = f_write(&m_fil, (void*)(img+i), 4, &NumBytesWritten);
//	}


	return 0;
}

int sd_write_bmp_packet(u8* packet, u32 size)
{
	int Res;
	Res = f_write(&m_fil, (void*)packet, size, &NumBytesWritten);
	if(Res)
		return 1;

	return 0;
}

int sd_write(const void* buffer, int size, int append)
{
	FRESULT Res;
	if (!append) {
		// Pointer to beginning of file .
		Res = f_lseek(&m_fil, 0);
		if (Res) {
			return XST_FAILURE;
		}
	}
	Res = f_write(&m_fil, (const void*)buffer, size,
						  &NumBytesWritten);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


int sd_close()
{
	FRESULT Res;
	Res = f_close(&m_fil);
	if (Res) {
		return XST_FAILURE;
	}
	return XST_SUCCESS;
}


void sd_bmp_test(struct pixel* IMG)
{
	//test loop
	uint8_t burn = 0x00;
	for(uint16_t i=0; i<128*128;++i)
	{
		f_write(&m_fil, (void*)(&(IMG[i].blue)), 1, &NumBytesWritten);
		f_write(&m_fil, (void*)(&(IMG[i].green)), 1, &NumBytesWritten);
		f_write(&m_fil, (void*)(&(IMG[i].red)), 1, &NumBytesWritten);
		f_write(&m_fil, (void*)(&burn), 1, &NumBytesWritten);
	}
}
