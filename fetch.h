#ifndef FETCH_H
#define FETCH_H

#include <time.h>

int download_image(const char *fmt, time_t time, char *file_path);
int download_weather_image(struct map *map, time_t time, char *file_path);

#endif
