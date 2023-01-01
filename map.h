#ifndef MAP_H
#define MAP_H

enum image_type
{
	radar,
	clouds,
	lightning,
	surface_temp,
	apparent_temp,
	high_temp,
	low_temp,
	short_hazards,
	long_hazards,
	fire_weather,
	surface_visibility,
	snow_fall,
	dew_point,
	algae,
	num_image_types
};

enum color
{
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	color_count,
	grey,
	light_grey,
	white,
	normal
};

struct map
{
	// File data
	int w; 			// Width in chars
	int h; 			// Height in chars
	int wpx; 		// Width in pixels
	int hpx;	 	// Height in pixels
	int startx; 		// starting pixel x
	int starty; 		// starting pixel y
	char *data; 		// ascii art map no newlines
	// Runtime data
	int panx; 		// Where to start printing from x
	int pany; 		// Where to start printing from y
	int renderw;		// Console width
	int renderh;		// Console height
	enum color *radar;	// Radar image in vt100 colors no newlines
	enum image_type image_type;
};

int load_map(struct map *map, char *filename);
void free_map(struct map *map);
void print_map(struct map *map, int reset);

#endif
