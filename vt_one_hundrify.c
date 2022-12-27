#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "vt_one_hundrify.h"
#include "readpng.h"
#include "rgbtohsv.h"
#include "map.h"

static png_structp pngptr = NULL;
static png_infop pnginfo = NULL;

static int hue_is_red(double h)
{
	return (h <= 360 && h > 350 || h <= 18 && h >= 0);
}

static int hue_is_orange(double h)
{
	return (h <= 45 && h > 18);
}

static int hue_is_yellow(double h)
{
	return (h <= 68 && h > 45);
}

static int hue_is_green(double h)
{
	return (h <= 145 && h > 68);
}

static int hue_is_cyan(double h)
{
	return (h <= 196 && h > 145);
}

static int hue_is_light_blue(double h)
{
	return (h <= 225 && h > 196);
}

static int hue_is_dark_blue(double h)
{
	return (h <= 256 && h > 225);
}

static int hue_is_purple(double h)
{
	return (h <= 350 && h > 256);
}

static int hue_is_vt100_red(double h)
{
	return hue_is_red(h) || hue_is_orange(h);
}

static int hue_is_vt100_green(double h)
{
	return hue_is_green(h);
}

static int hue_is_vt100_yellow(double h)
{
	return hue_is_yellow(h);
}

static int hue_is_vt100_blue(double h)
{
	return hue_is_light_blue(h) || hue_is_dark_blue(h);
}

static int hue_is_vt100_magenta(double h)
{
	return hue_is_purple(h);
}

static int hue_is_vt100_cyan(double h)
{
	return hue_is_cyan(h);
}

const char* color_to_vt100(enum color color)
{
	switch(color)
	{
		case red:
			return KRED;
		case green:
			return KGRN;
		case yellow:
			return KYEL;
		case blue:
			return KBLU;
		case magenta:
			return KMAG;
		case cyan:
			return KCYN;
		case normal:
			return KNRM;
	}
	return "";
}

/*void libpng_init (void)
{
	pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    	pnginfo = png_create_info_struct(pngptr);
        setjmp(png_jmpbuf(pngptr));
	//png_set_palette_to_rgb(pngptr);
	//png_set_expand(pngptr);
	//png_set_tRNS_to_alpha(pngptr);
   //png_colorp palette = (png_colorp)png_malloc(pngptr, 256 * sizeof (png_color));
   //png_set_PLTE(pngptr, pnginfo, palette, 256);
}*/

int vt_one_hundrify(struct map *map, char *filename)
{
	struct png_data png = read_png_file(filename);
/*
	FILE *fp = fopen(filename, "rb");
	png_init_io(pngptr, fp);
        png_read_info(pngptr, pnginfo);

	png_bytepp rows;
	png_read_png(pngptr, pnginfo, PNG_TRANSFORM_IDENTITY, NULL);
	printf("got hereeeee\n");
	rows = png_get_rows(pngptr, pnginfo);
	printf("got hereeeee\n");
	fclose(fp);
*/

	if (4 != png_get_channels(png.png, png.info))
		return -1; // not 4 channels
	if (map->hpx != png_get_image_height(png.png, png.info))
		return -2; // not the expected width
	if (map->wpx != png_get_image_width(png.png, png.info))
		return -3; // not the expected height

	/*for (int i = 0; i < map->hpx; i++) {
        for (int j = 0; j < map->wpx * 4; j += 4) {
		int hist[6] = { 0, 0, 0, 0, 0, 0 };
		if (rows[i][j + 3] == 255) {
			struct HSV hsv = rgb_to_hsv(rows[i][j], rows[i][j + 1], rows[i][j + 2]);
			printf("%s\n", hue_to_vt100(hsv.H));
			//printf("%f %f %f\n", hsv.H, hsv.S, hsv.V);
		}
        } */
    int h = map->hpx;
    int w = map->wpx;
    int mH = map->h;
    int mW = map->w;
    //printf("color count %d\n", color_count);
    for(int row=0; row < mH; row++) {
        for(int col=0; col < mW; col++) {
            int count[color_count];
            memset(count, 0, color_count*sizeof(int));
    //printf("row: %d col: %d map->w %d\n", row, col, map->w);
    //fflush(stdout);

            for(int y=row*h/mH; y < (row+1)*h/mH; y++) {
                for(int x=col*w/mW; x < (col+1)*w/mW; x++) {
  			  png_bytep img_row = png.image[y];
                          png_bytep px = &(img_row[x * 4]);
			  int r= px[0];
			  int b= px[1];
			  int g= px[2];
			  int a= px[3];
                          if(a== 255) {

                          //if(rows[y][x+3] > 0 && rows[y][x+3] < 253 || rows[y][x+3] > 253) {
                              double h = rgb_to_hsv(r, b, g).H;
		          //printf("x: %d y: %d r: %d g: %d b: %d a: %d\n", x/4, y, rows[y][x+0], rows[y][x+1], rows[y][x+2], rows[y][x+3]);

                              if (hue_is_vt100_red(h))
                                  count[red]++;
                              else if (hue_is_vt100_green(h))
                                  count[green]++;
                              else if (hue_is_vt100_yellow(h))
                                  count[yellow]++;
                              else if (hue_is_vt100_blue(h))
                                  count[blue]++;
                              else if (hue_is_vt100_magenta(h))
                                  count[magenta]++;
                              else if (hue_is_vt100_cyan(h))
                                  count[cyan]++;
			  }
                }
            }

            int max=0;
            enum color color=0;
            for(int i=0; i<color_count;i++) {
                if(count[i] > max){
                    max = count[i];
                    color = i;
                }
            }

            if(max != 0) {
		map->radar[row*map->w + col] = color;
            }
            else {
		//printf("%d %d %d colored\n", col, row, map->w );
		map->radar[row*map->w + col] = normal;
            }
        }
    }
    free_image(png);
    return 0;
}
