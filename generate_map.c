#include <png.h>
#include <stdio.h>
#include <string.h>

#include "readpng.h"

// Each 3x3 image represents the ascii char listed
// Assume that images perform extra erossion.
// In other words account for cases where pixels are missing,
// but do not account for cases of extra pixels
// 10 3x3 images for all characters

struct block {
	int populated;
	int block[3][3];
};

static struct block char_blocks [256][10] =
{
	['\\'] = {
	  {1, {{1, 0, 0},
	       {0, 1, 0},
	       {0, 0, 1}}},
	  {1, {{1, 0, 0},
	       {0, 0, 0},
	       {0, 0, 1}}},
	  {1, {{1, 0, 0},
	       {0, 1, 0},
	       {0, 0, 0}}},
	},
	['/'] = {
	  {1, {{0, 0, 1},
	       {0, 1, 0},
	       {1, 0, 0}}},
	  {1, {{0, 0, 1},
	       {0, 0, 0},
	       {1, 0, 0}}},
	  {1, {{0, 0, 1},
	       {0, 1, 0},
	       {0, 0, 0}}},
	},
	['|'] = {
	  {1, {{0, 1, 0},
	       {0, 1, 0},
	       {0, 1, 0}}},
	  {1, {{0, 1, 0},
	       {0, 0, 0},
	       {0, 1, 0}}},
	  {1, {{0, 1, 0},
	       {0, 1, 0},
	       {0, 0, 0}}},
	  {1, {{0, 0, 0},
	       {0, 1, 0},
	       {0, 1, 0}}},
	},
	[','] = {
	  {1, {{0, 0, 0},
	       {0, 1, 0},
	       {1, 0, 0}}},
	},
	['.'] = {
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {1, 0, 0}}},
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {0, 1, 0}}},
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {0, 0, 1}}},
	},
	['_'] = {
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {1, 1, 1}}},
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {1, 1, 0}}},
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {0, 1, 1}}},
	  {1, {{0, 0, 0},
	       {0, 0, 0},
	       {1, 0, 1}}},
	},
	['-'] = {
	  {1, {{0, 0, 0},
	       {1, 1, 1},
	       {0, 0, 0}}},
	  {1, {{0, 0, 0},
	       {1, 1, 0},
	       {0, 0, 0}}},
	  {1, {{0, 0, 0},
	       {0, 1, 1},
	       {0, 0, 0}}},
	  {1, {{0, 0, 0},
	       {1, 0, 1},
	       {0, 0, 0}}},
	},
	['+'] = {
	  {1, {{0, 1, 0},
	       {1, 1, 1},
	       {0, 1, 0}}},
	  {1, {{0, 1, 0},
	       {1, 0, 1},
	       {0, 1, 0}}},
	},
	['\''] = {
	  {1, {{0, 0, 1},
	       {0, 0, 0},
	       {0, 0, 0}}},
	  {1, {{0, 1, 0},
	       {0, 0, 0},
	       {0, 0, 0}}},
	  {1, {{0, 1, 1},
	       {0, 0, 0},
	       {0, 0, 0}}},
	},
	['`'] = {
	  {1, {{1, 0, 0},
	       {0, 0, 0},
	       {0, 0, 0}}},
	},
	[')'] = {
	  {1, {{0, 1, 0},
	       {0, 0, 1},
	       {0, 1, 0}}},
	  {1, {{0, 1, 1},
	       {0, 0, 1},
	       {0, 1, 1}}},
	  {1, {{1, 1, 1},
	       {0, 0, 1},
	       {1, 1, 1}}},
	},
	['('] = {
	  {1, {{0, 1, 0},
	       {1, 0, 0},
	       {0, 1, 0}}},
	},
	['^'] = {
	  {1, {{0, 1, 0},
	       {1, 0, 1},
	       {0, 0, 0}}},
	},
	['v'] = {
	  {1, {{0, 0, 0},
	       {1, 0, 1},
	       {0, 1, 0}}},
	},
	['o'] = {
	  {1, {{0, 0, 0},
	       {0, 1, 1},
	       {0, 1, 1}}},
	  {1, {{0, 0, 0},
	       {1, 1, 0},
	       {1, 1, 0}}},
	},
	['O'] = {
	  {1, {{1, 1, 1},
	       {1, 0, 1},
	       {1, 1, 1}}},
	  {1, {{0, 1, 1},
	       {1, 0, 1},
	       {1, 1, 1}}},
	  {1, {{1, 1, 0},
	       {1, 0, 1},
	       {1, 1, 1}}},
	  {1, {{1, 1, 1},
	       {1, 0, 1},
	       {1, 1, 0}}},
	  {1, {{1, 1, 1},
	       {1, 0, 1},
	       {0, 1, 1}}},
	},
	['C'] = {
	  {1, {{1, 1, 0},
	       {1, 0, 0},
	       {1, 1, 0}}},
	  {1, {{1, 1, 1},
	       {1, 0, 0},
	       {1, 1, 1}}},
	},
};

void max_grey_png(struct png_data png)
{
	int channels = png_get_channels(png.png, png.info);
	int h = png_get_image_height(png.png, png.info);
	int w = png_get_image_width(png.png, png.info);

	for (int i = 0; i < h; i++)
	for (int j = 0; j < w; j++) {
		int max_val = 0;
		png_bytep img_row = png.image[i];
		png_bytep px = &(img_row[j * channels]);

		for (int c = 0; c < channels-1; c++)
			max_val = px[c] > max_val ? px[c] : max_val;

		for (int c = 0; c < channels-1; c++)
			px[c] = max_val;

	}
}

int main(void)
{
	struct png_data png = read_png_file("counties.png");
	int channels = png_get_channels(png.png, png.info);
	int h = png_get_image_height(png.png, png.info);
	int w = png_get_image_width(png.png, png.info);

	//max_grey_png(png);

	for (int row = 0; row < h/3; row++) {
		for (int col = 0; col < w/3; col++) {
			int y = row * 3;
			int x = col * 3;
			int v = 0;
			int has_pixel = 0;

			//printf("========================== processing pixel %d %d\n", y, x);
			for (int i=0; i < 3; i++) {
				for (int j=0; j < 3; j++) {
					//printf("processing pixel %d %d\n", y+i, x+j);
					png_bytep img_row = png.image[y+i];
					png_bytep px = &(img_row[(x+j) * channels]);

					if (px[0] > 200)
						has_pixel = 1;
				}
			}

			if(has_pixel) {
				// Number of matching pixels
				int match_count[256];
				memset(match_count, 0, 256*sizeof(int));

				for (int c = 0; c < 256; c++) {
					if (char_blocks[c][0].populated) {
						for (int b = 0; b < 10; b++) {
							if (char_blocks[c][b].populated) {
								for (int i=0; i < 3; i++) {
									for (int j=0; j < 3; j++) {
										// Compare pixels
										png_bytep img_row = png.image[y+i];
										png_bytep px = &(img_row[(x+j) * channels]);
										// Threshold
										if (px[0] > 200) {
											if (char_blocks[c][b].block[i][j])
												match_count[c]++;
										}
										else {
											if (!char_blocks[c][b].block[i][j])
												match_count[c]++;
										}
									}
								}
							}
						}
					}
				}

				// Select char with the most matching pixels
				int max = 0;
				int max_char = 0;
				for(int i=0; i<256;i++) {
					if(match_count[i] > max){
						max = match_count[i];
						max_char = i;
					}
				}

				if (max_char)
					printf("%c", max_char);
			}
			else
				printf(" ");
		}
		printf("\n");
	}


	free_image(png);
	return 0;
}
