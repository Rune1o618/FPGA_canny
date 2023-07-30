#ifndef __BMP_H
#define __BMP_H

struct pixel* read_data(FILE* stream, const struct bmp_header* header){
  if(stream == NULL || header == NULL){
    return 0;
  }
  // w == 1 && p == 1;   w == 2 && p == 2;   w == 3 && p == 3;   w == 4 && p == 0
  int padding = header->width % 4;
  int num_of_pixels = header->width * header->height;
  struct pixel* Pixel[num_of_pixels];

  fseek(stream, 54, SEEK_SET); //move 54B (header size)
  int index_p = 0;
  for(int i = 0; i < header->height; i++){
    for(int j = 0; j < header->width; j++){
      Pixel[index_p] = malloc(sizeof(struct pixel));
      fread(&(Pixel[index_p]->blue), 1, 1, stream);
      fread(&(Pixel[index_p]->green), 1, 1, stream);
      fread(&(Pixel[index_p]->red), 1, 1, stream);
      index_p++;
    }
    fseek(stream, padding, SEEK_CUR);  //padding at the end of row
  }
  return *Pixel;
}

#endif
