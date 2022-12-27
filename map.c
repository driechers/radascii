#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "vt_one_hundrify.h"

int load_map(struct map *map, char *filename)
{
	int h;
	// TODO error check
	FILE *fp = fopen(filename, "r");
	// TODO load file data
	map->h = 120;
	map->w = 470;
	map->hpx = 513;
	map->wpx = 941;
	// TODO use startx and starty in vt on hundrify
	map->startx = 0;
	map->starty = 0;

	// Init runtime data
	map->panx = 0;
	map->pany = 0;
	map->radar = malloc(sizeof(enum color) * map->h * map->w);
	for(int i=0; i < map->h * map->w; i++)
		map->radar[i] = normal;

	map->data = malloc(map->h * map->w);
	for(h=0; h < map->h; h++) {
		char lastchar = 0;
		if(map->w != fread(map->data + h*map->w, 1, map->w, fp))
			return -1; // Failed to read w chars
		fread(&lastchar, 1, 1, fp);
		if (lastchar != '\n') {
			fprintf(stderr, "Line %d: Last char was %c 0x%02x\n", h+1, lastchar, lastchar);
			return -2; // Last char in line was not \n
		}
	}
	if (h != map->h)
		return -3; // Height is not correct
	if (EOF != fgetc(fp))
		return -4; // We should be at eof
	if (ferror(fp))
		return -5; // An error occured

	fclose(fp);
}

void free_map(struct map *map)
{
	free(map->data);
	free(map->radar);
}

void print_map(struct map *map)
{
	// TODO adjust to console size
	for (int j=map->pany; j < map->pany + 300 && j < map->h; j++) {
		for (int i=map->panx; i < map->panx + 140 && i < map->w; i++) {
			printf("%s%c", color_to_vt100(map->radar[j*map->w +i]), map->data[j*map->w + i]);
		}
		putchar('\n');
	}
}

