#include <png.h>
#include <stdlib.h>
#include <string.h>

#include "vt_one_hundrify.h"
#include "readpng.h"
#include "rgbtohsv.h"
#include "map.h"

static png_structp pngptr = NULL;
static png_infop pnginfo = NULL;

// TODO support full color range if terminal supports it

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

// TODO it would be cool if the code was smart enough to pick out values in the image/key instead of hard coding range
static int shade_is_vt100_grey(int s)
{
	return s > 64 && s < 128;
}

static int shade_is_vt100_light_grey(int s)
{
	return s > 128 && s < 192;
}

static int shade_is_vt100_white(int s)
{
	return s > 192;
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
		case white:
			return KWHT;
		case light_grey:
			return KLGRY;
		case grey:
			return KGRY;
		case normal:
			return KNRM;
	}
	return "";
}

int vt_one_hundrify_generic(struct map *map, char *filename)
{
	int h = map->hpx;
	int w = map->wpx;
	int mH = map->h;
	int mW = map->w;
	struct png_data png = read_png_file(filename);

	if (4 != png_get_channels(png.png, png.info))
		return -1; // not 4 channels
	if (map->hpx != png_get_image_height(png.png, png.info)) {
		printf("height %d expected %d\n", png_get_image_height(png.png, png.info), map->hpx);
		return -2; // not the expected height
	}
	if (map->wpx != png_get_image_width(png.png, png.info))
		return -3; // not the expected width

	for(int row=0; row < mH; row++)
	for(int col=0; col < mW; col++) {
		int count[color_count];
		memset(count, 0, color_count*sizeof(int));

		// startx and starty have limited testing
		for(int y=row*h/mH + map->starty; y < (row+1)*h/mH + map->starty; y++)
		for(int x=col*w/mW + map->startx; x < (col+1)*w/mW + map->startx; x++) {
			png_bytep img_row = png.image[y];
			png_bytep px = &(img_row[x * 4]);
			int r= px[0];
			int b= px[1];
			int g= px[2];
			int a= px[3];
			if(a== 255) {
				if(r == b && b == g) {
					if (shade_is_vt100_white(r))
						count[white]++;
					if (shade_is_vt100_light_grey(r))
						count[light_grey]++;
					if (shade_is_vt100_grey(r))
						count[grey]++;
				}
				else {
					double h = rgb_to_hsv(r, b, g).H;

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

		if(max != 0)
			map->radar[row*map->w + col] = color;
		else
			map->radar[row*map->w + col] = normal;
	}
	free_image(png);
	return 0;
}

int vt_one_hundrify_clouds(struct map *map, char *filename)
{
	int h = map->hpx;
	int w = map->wpx;
	int mH = map->h;
	int mW = map->w;
	struct png_data png = read_png_file(filename);

	if (4 != png_get_channels(png.png, png.info))
		return -1; // not 4 channels
	if (map->hpx != png_get_image_height(png.png, png.info))
		return -2; // not the expected width
	if (map->wpx != png_get_image_width(png.png, png.info))
		return -3; // not the expected height

	for(int row=0; row < mH; row++)
	for(int col=0; col < mW; col++) {
		int cloud_pixels = 0;
		int total = 0;

		// startx and starty have limited testing
		for(int y=row*h/mH + map->starty; y < (row+1)*h/mH + map->starty; y++)
		for(int x=col*w/mW + map->startx; x < (col+1)*w/mW + map->startx; x++) {
			png_bytep img_row = png.image[y];
			png_bytep px = &(img_row[x * 4]);
			int r= px[0];
			int b= px[1];
			int g= px[2];
			int a= px[3];
			if(a== 255 && r == b && b == g) {
				// This should be all pixels
				if (r > 128)
					cloud_pixels++;
			}
			total++;
		}

		double cloud_ratio = (double)cloud_pixels / (double)total;

		if(cloud_ratio > 3./4.)
			map->radar[row*map->w + col] = white;
		else if(cloud_ratio > 1./2.)
			map->radar[row*map->w + col] = light_grey;
		else if(cloud_ratio > 1./4.)
			map->radar[row*map->w + col] = grey;
		else
			map->radar[row*map->w + col] = normal;
	}
	free_image(png);
	return 0;
}

// TODO generate county map from https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_pts_zones_geolinks/MapServer/export?dpi=1&transparent=true&format=png32&layers=show%3A9&bbox=-15290014.707599312%2C2603370.6826554714%2C-6083327.524708849%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=3764%2C2052&f=image
// TODO possibly change satelite image to skycover since that is analyzed and could allow for simple desampling

int vt_one_hundrify(struct map *map, char *filename)
{
	if(map->image_type == clouds)
		vt_one_hundrify_clouds(map, filename);
	else
		vt_one_hundrify_generic(map, filename);
}
