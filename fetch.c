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

int download_radar_image(struct map *map, unsigned long long time, char *file_path)
{
	// TODO use position / url data from map
	return download_image("https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C1671846900000&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image", time, file_path);
}

// TODO same as radar but with cloud cover
//downloadCloudImage
