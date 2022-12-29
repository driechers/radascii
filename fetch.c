#include <curl/curl.h>

#include "map.h"
#include "fetch.h"

// TODO error checking
int download_image(char *fmt, unsigned long long time, char *file_path)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char url[512];
	char fileName[11];

	snprintf(url, 512, fmt, time);

	curl = curl_easy_init();
	if (curl) {
		fp = fopen(file_path,"wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.81.0");
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return 0;
}

int download_weather_image(struct map *map, unsigned long long time, char *file_path, int clouds)
{
	if(clouds)
		return download_image(map->cloud_url, time, file_path);
	else
		return download_image(map->radar_url, time, file_path);

}
