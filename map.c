#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "vt_one_hundrify.h"

int load_map(struct map *map, char *filename)
{
	int h;
	char seperator[32];
	// TODO error check
	FILE *fp = fopen(filename, "r");
	fgets(seperator, 32, fp);
	fscanf(fp, "width: %d\n", &map->w);
	fscanf(fp, "height: %d\n", &map->h);
	fscanf(fp, "image width: %d\n", &map->wpx);
	fscanf(fp, "image height: %d\n", &map->hpx);
	fscanf(fp, "start pixel x: %d\n", &map->startx);
	fscanf(fp, "start pixel y: %d\n", &map->starty);
	fgets(seperator, 32, fp);

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

void print_map(struct map *map, int reset)
{
	if (reset) {
		printf("\r%s", KCHM);
	}
	// TODO adjust to console size
	for (int j=map->pany; j < map->pany + map->renderh && j < map->h; j++) {
		for (int i=map->panx; i < map->panx + map->renderw && i < map->w; i++) {
			printf("%s%c", color_to_vt100(map->radar[j*map->w +i]), map->data[j*map->w + i]);
		}
		printf("%s\r\n", KNRM);
	}
}

