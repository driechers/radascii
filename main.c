#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "map.h"
#include "fetch.h"
#include "vt_one_hundrify.h"

static const char * image_types[] = {
	[radar] = "radar",
	[clouds] = "clouds",
	[lightning] = "lightning",
	[surface_temp] = "surface_temp",
	[apparent_temp] = "apparent_temp",
	[high_temp] = "high_temp",
	[low_temp] = "low_temp",
	[short_hazards] = "short_hazards",
	[long_hazards] = "long_hazards",
	[fire] = "fire",
	[surface_visibility] = "visibility",
	[snow_fall] = "6hr_snow_fall",
	[dew_point] = "dew_point",
	[algae] = "harmful_algae_bloom",
	[relative_humidity] = "relative_humidity",
	[wind_speed] = "wind_speed",
	[wind_gust] = "wind_gust",
	[wave] = "wave",
	[precipitation_prob] = "12hr_precipitation_probability",
	[one_hr_accumulation] = "1hr_accumulation",
	[three_hr_accumulation] = "3hr_accumulation",
	[six_hr_accumulation] = "6hr_accumulation",
	[twelve_hr_accumulation] = "12hr_accumulation",
	[twentyfour_hr_accumulation] = "24hr_accumulation",
	[fortyeight_hr_accumulation] = "48hr_accumulation",
	[seventytwo_hr_accumulation] = "72hr_accumulation"




};

void show_image_types(void)
{
	printf("image types:\n");
	for(int i=0; i < num_image_types; i++) {
		printf("\t%s\n", image_types[i]);
	}
}

void key(void)
{
	printf(
		"0    10   20   30   40   50   60   70   80   DBZ\n"
		"   " KBLU "     " KCYN "    " KGRN "       " KYEL "      " KRED "     " KMAG "                  " KNRM "\n\n\n"
		"0 .1     2.5    25          150  200    300   (strikes/km^2)/min x 10^3\n"
		KNRM "   " KYEL "      " KRED "       " KMAG "            " KBLU "     " KCYN "       " KGRN "        " KNRM "\n"
		"0 .2     7      60          370  500    777   (strikes/mi^2)/min x 10^3\n\n\n"

		"No Clouds    Partly Cloudy    Mostly Cloudy    Overcast\n"
		"             " KGRY "                 " KLGRY "                 " KWHT "        " KNRM "\n"
	      );
}


//TODO support animation
struct args
{
	enum image_type image_type;
	int loop;
	int animated;
	char *map_file;
	char *test_image;
	int x;
	int y;
	int w;
	int h;
	int interactive;
};

void usage()
{
	printf("Usage:\n\n"
		"radascii [options]\n"
		"\tDefault behavior is to display the most recent radar image over Us Mexico Canida map.\n"
		"\t-t <test radar png file>\n"
		"\t-m <ascii map file>\n"
		"\t-x <x coord to pan to>\n"
		"\t-y <y coord to pan to>\n"
		"\t-r <console rows>\n"
		"\t-c <console columns>\n"
		"\t-l list image types.\n"
		"\t-i <image type>\n"
		"\t-h Dispaly this menu.\n"
		"\t-k Dispaly the map key.\n"
		"\t-a Play radar animation once.\n"
		"\t-I Interactive mode arrows to move map and q to quit.\n"
	      );
}

void getOptions(struct args *args, int argc, char **argv)
{
	int opt;

	// Defaults
	char *str_image_type = "radar";
	char *str_x = "0";
	char *str_y = "0";
	char *str_c = "80";
	char *str_r = "24";
	args->animated = 0;
	args->interactive = 0;
	args->loop = 0;
	args->map_file = "usmca.txt";
	args->test_image = NULL;

	// Process Args
	// TODO long options
	while((opt=getopt(argc, argv, "zlahkIm:t:x:y:w:h:c:r:i:")) != -1 )  {
		switch(opt) {
			case 'a':
				args->animated = 1;
				break;
			case 'z':
				args->loop = 1;
				break;
			case 'I':
				args->interactive = 1;
				break;
			case 'h':
				usage();
				exit(0);
			case 'm':
				args->map_file = optarg;
				break;
			case 't':
				args->test_image = optarg;
				break;
			case 'x':
				str_x = optarg;
				break;
			case 'y':
				str_y = optarg;
				break;
			case 'c':
				str_c = optarg;
				break;
			case 'r':
				str_r = optarg;
				break;
			case 'i':
				str_image_type = optarg;
				break;
			case 'k':
				key();
				exit(0);
			case 'l':
				show_image_types();
				exit(0);
			default:
				usage();
				exit(-1);
		}
	}

	// Convert args
	args->x = atoi(str_x);
	args->y = atoi(str_y);
	args->w = atoi(str_c);
	args->h = atoi(str_r);

	for(args->image_type = radar; args->image_type < num_image_types; args->image_type++) {
		if(0 == strcmp(image_types[args->image_type], str_image_type))
			break;
	}

	if(args->image_type == num_image_types) {
		printf("image type does not exist\n\n");
		show_image_types();
		exit(-1);
	}
}

static struct map map[10];
static int num_frames= 1;
static int done = 0;

void* display_frames(void *thread_args)
{
	int panx = map[0].panx;
	int pany = map[0].pany;
	while(!done) {
		panx = map[0].panx;
		pany = map[0].pany;

		for (int frame=0; frame < num_frames; frame++) {
			print_map(&map[frame], 1);
			// TODO time of image
			printf("q=quit  r=refresh space=next image  h=left  j=down  k=up  l=right  %s            ",
				image_types[map[0].image_type] );
			if(num_frames > 1) {
				// One second sleep with early out if the map is panned
				for(int i = 0; i < 1000; i++) {
					if(done || panx != map[0].panx || pany != map[0].pany)
						goto stop_rendering;
					usleep(1000);
				}
			}
		}
stop_rendering:
	}

	return NULL;
}

// TODO Provide status message for download
// TODO top level error checking
int main(int argc, char **argv)
{
	pthread_t display_thread;
	void * thread_ret;
	struct args args;
	int r = 0;
	char img_dir[]="/tmp/tmp.XXXXXX";
	char img_path[512];
	int frame = 0;

	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long now =
		(unsigned long long)(tv.tv_sec) * 1000 +
		(unsigned long long)(tv.tv_usec) / 1000;

	getOptions(&args, argc, argv);

	if(args.animated)
		num_frames = 10;

	// Create dir for frames
	mkdtemp(img_dir);

	frame = 0;
	do {
		r = load_map(&map[frame], args.map_file);
		map[frame].image_type = args.image_type;

		if(args.test_image) {
			r = vt_one_hundrify(&map[frame], args.test_image);
		}
		else {
			unsigned long long frame_time = now;

			if (args.animated)
				frame_time -= (num_frames - frame - 1)*15*1000*60;

			sprintf(img_path, "%s/%d.png", img_dir, num_frames - frame - 1);

			r = download_weather_image(&map[frame], frame_time, img_path);
			r = vt_one_hundrify(&map[frame], img_path);
    			remove(img_path);
		}

		map[frame].panx = args.x;
		map[frame].pany = args.y;
		map[frame].renderw = args.w;
		map[frame].renderh = args.h;
	} while (++frame < num_frames);

	if(args.interactive) {
		system("stty raw -echo");
		printf(KCHD);
		pthread_create(&display_thread, NULL, display_frames, NULL);

		int key = 0;
		while (key != 'q') { // TODO also esc
			struct timeval tv;
			gettimeofday(&tv, NULL);
			unsigned long long now =
				(unsigned long long)(tv.tv_sec) * 1000 +
				(unsigned long long)(tv.tv_usec) / 1000;

			// TODO print in different thread
			key = getchar();
			for (int frame=0; frame < num_frames; frame++) {
				// TODO toggle animation
				switch(key) {
					case 'k': // TODO also up arrow
						if (map[frame].pany > 0)
							map[frame].pany--;
						break;
					case 'j': // TODO also down arrow
						if (map[frame].pany + map[frame].renderh < map[frame].h)
							map[frame].pany++;
						break;
					case 'h': // TODO also left arrow
						if (map[frame].panx > 0)
							map[frame].panx--;
						break;
					case 'l': // TODO also right arrow
						if (map[frame].panx + map[frame].renderw < map[frame].w)
							map[frame].panx++;
						break;
					case ' ':
						map[frame].image_type = map[frame].image_type == num_image_types-1 ? 
							0: map[frame].image_type + 1;
					case 'r':
						// This should probably be cleaner
						sprintf(img_path, "%s/%d.png", img_dir, frame);

						r = download_weather_image(&map[frame], now-frame*15*1000*60, img_path);
						r = vt_one_hundrify(&map[frame], img_path);
    						remove(img_path);
						break;
				}
			}
		}
		done = 1;
		pthread_join(display_thread, &thread_ret);
		system("stty cooked echo");
		printf("\n"KCSW);
	}
	else {
		do {
			frame = 0;
			do {
				print_map(&map[frame], args.animated);
				if(args.animated)
					sleep(1);
			} while (++frame < num_frames);
		} while(args.loop);
	}

	remove(img_dir);

	frame = 0;
	do {
		free_map(&map[frame]);
	} while (++frame < num_frames);

	return 0;
}

// TODO break up file
// TODO handle sigterm
