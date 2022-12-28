#ifndef MAP_H
#define MAP_H

enum color
{
	red,
	green,
	yellow,
	blue,
	magenta,
	cyan,
	color_count,
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
	char radar_url[512];	// URL to download radar image
	char *data; 		// ascii art map no newlines
	// Runtime data
	int panx; 		// Where to start printing from x
	int pany; 		// Where to start printing from y
	int renderw;		// Console width
	int renderh;		// Console height
	enum color *radar;	// Radar image in vt100 colors no newlines
};

int load_map(struct map *map, char *filename);
void free_map(struct map *map);
void print_map(struct map *map);

#endif
