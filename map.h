#ifndef MAP_H
#define MAP_H

struct map
{
	// File data
	int h; 		// Height in chars
	int w; 		// Width in chars
	int hpx; 	// Height in pixels
	int wpx; 	// Width in pixels
	int startx; 	// starting pixel x
	int starty; 	// starting pixel y
		    	// TODO nowcoast position
	char *data; 	// ascii art map no newlines
	// Runtime data
	int panx; 	// Where to start printing from x
	int pany; 	// Where to start printing from y
	char *radar;	// Radar image in vt100 colors no newlines
};

#endif
