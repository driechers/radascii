/*
 * A simple libpng example program
 * http://zarb.org/~gc/html/libpng.html
 *
 * Modified by Yoshimasa Niwa to make it much simpler
 * and support all defined color_type.
 * Modified by Daniel Riechers to contain only a
 * function to read in png to array
 *
 * To build, use the next instruction on OS X.
 * $ brew install libpng
 * $ clang -lz -lpng15 libpng_test.c
 *
 * Copyright 2002-2010 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <png.h>

#include "readpng.h"

struct png_data read_png_file(char *filename) {
	int width, height;
	png_byte color_type;
	png_byte bit_depth;
	png_bytep *row_pointers;
	FILE *fp = fopen(filename, "rb");

	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png) abort();

	png_infop info = png_create_info_struct(png);
	if(!info) abort();

	if(setjmp(png_jmpbuf(png))) abort();

	png_init_io(png, fp);

	png_read_info(png, info);

	width      = png_get_image_width(png, info);
	height     = png_get_image_height(png, info);
	color_type = png_get_color_type(png, info);
	bit_depth  = png_get_bit_depth(png, info);

	// Read any color_type into 8bit depth, RGBA format.
	// See http://www.libpng.org/pub/png/libpng-manual.txt

	if(bit_depth == 16)
		png_set_strip_16(png);

	if(color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);

	// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);

	if(png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	// These color_type don't have an alpha channel then fill it with 0xff.
	if(color_type == PNG_COLOR_TYPE_RGB ||
	   color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

	if(color_type == PNG_COLOR_TYPE_GRAY ||
	   color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);

	png_read_update_info(png, info);

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
	for(int y = 0; y < height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));

	png_read_image(png, row_pointers);

	fclose(fp);
	struct png_data ret = {
		png,
		info,
		row_pointers
	};

	return ret;
}

void free_image(struct png_data png_data)
{
	int width      = png_get_image_width(png_data.png, png_data.info);
	int height     = png_get_image_height(png_data.png, png_data.info);

	for(int y=0; y < height; y++)
		free(png_data.image[y]);
	free(png_data.image);

	png_destroy_info_struct(png_data.png, &png_data.info);
	png_destroy_read_struct(&png_data.png, NULL, NULL);
}
