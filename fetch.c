#include <curl/curl.h>

#include "map.h"
#include "fetch.h"

// Gridded 
// TODO relative humidity
// TODO significant wave height
// TODO 12 hr probability of preciptiation
// Surface weather analysis
// TODO wind speed
// TODO wind gust
// Analysis
// TODO precipitation url but allow selection of time passed
static const char * image_type_urls[] = {
	[radar] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/radar_meteo_imagery_nexrad_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[clouds] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/sat_meteo_imagery_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A11%%2C23&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[lightning] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/sat_meteo_emulated_imagery_lightningstrikedensity_goes_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A3&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[surface_temp] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/analysis_meteohydro_sfc_rtma_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A11&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[apparent_temp] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_sfc_ndfd_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A39&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[high_temp] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_sfc_ndfd_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A7&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[low_temp] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_sfc_ndfd_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A11&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[surface_visibility] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/analysis_meteohydro_sfc_rtma_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A27&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[long_hazards] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/wwa_meteoceanhydro_longduration_hazards_time/MapServer/export?dpi=96&transparent=true&format=png8&layers=show%%3A1%%2C4%%2C8%%2C11%%2C15%%2C18%%2C22%%2C25%%2C28%%2C34%%2C31%%2C37%%2C40&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[short_hazards] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/wwa_meteoceanhydro_shortduration_hazards_watches_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A0%%2C1%%2C2&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[snow_fall] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_sfc_ndfd_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A19&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[dew_point] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/analysis_meteohydro_sfc_rtma_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A15&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[algae] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_pts_zones_geolinks/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A3&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image",
	[relative_humidity] = "https://nowcoast.noaa.gov/arcgis/rest/services/nowcoast/forecast_meteoceanhydro_sfc_ndfd_time/MapServer/export?dpi=96&transparent=true&format=png32&layers=show%%3A43&time=%llu%%2C%llu&bbox=-15290014.707599312%%2C2603370.6826554714%%2C-6083327.524708849%%2C7622531.70797195&bboxSR=3857&imageSR=3857&size=941%%2C513&f=image"
};

// TODO error checking
int download_image(const char *fmt, unsigned long long time, char *file_path)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char url[512];
	char fileName[11];

	snprintf(url, 512, fmt, time, time);

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
