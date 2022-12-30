#ifndef FETCH_H
#define FETCH_H

int download_image(char *fmt, unsigned long long time, char *file_path);
int download_weather_image(struct map *map, unsigned long long time, char *file_path);

#endif
