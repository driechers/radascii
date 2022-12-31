#include <curl/curl.h>

#include "map.h"
#include "fetch.h"

static const char * image_type_urls[] = {
    [radar] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C1671846900000&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
    [clouds] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/sat_meteo_imagery_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A11%%2C23&time=%llu%%2C1672262280000&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
    [lightning] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/sat_meteo_emulated_imagery_lightningstrikedensity_goes_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C1672351200000&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image"
};

// TODO error checking
int download_image(const char *fmt, unsigned long long time, char *file_path)
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

int download_weather_image(struct map *map, unsigned long long time, char *file_path)
{
	if(image_type_urls[map->image_type])
		return download_image(image_type_urls[map->image_type], time, file_path);

	return -1;
}
