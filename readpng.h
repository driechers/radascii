#ifndef READPNG_H
#define READPNG_H

struct png_data
{
  	png_structp png;
	png_infop info;
	png_bytep *image;
};

struct png_data read_png_file(char *filename);
void free_image(struct png_data png_data);

#endif
