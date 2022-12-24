#include <curl/curl.h>
#include <sys/time.h>

#include "map.h"

// TODO error checking
// TODO rewrite to download png to file_path
void downloadRadarImage(struct map *map, unsigned long long time_offset, char *file_path) {
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char url[512];
	char fileName[11];

	struct timeval tv;

	gettimeofday(&tv, NULL);
	unsigned long long millisecondsSinceEpoch =
		(unsigned long long)(tv.tv_sec) * 1000 +
		(unsigned long long)(tv.tv_usec) / 1000;

	// TODO load format string from map
	snprintf(url, 512,  "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C1671846900000&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image", millisecondsSinceEpoch - time_offset);
	printf("%s\n", url);



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
}

// TODO same as radar but with cloud cover
//char* downloadCloudImage(struct map *map, int time_offset) {

int main(void)
{
	downloadRadarImage(NULL, 0, "test.png");
	downloadRadarImage(NULL, 3600000, "test1.png");
	downloadRadarImage(NULL, 3600000*2, "test2.png");
	downloadRadarImage(NULL, 3600000*3, "test3.png");

	return 0;
}
