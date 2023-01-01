#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
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
	[fire_weather] = "fire",
	[surface_visibility] = "visibility",
	[snow_fall] = "6hr_snow_fall",
	[dew_point] = "dew_point",
	[algae] = "harmful_algae_bloom",
	[relative_humidity] = "relative_humidity"
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

#define KWHT  "\x1B[30m\x1B[107m"
#define KLGRY "\x1B[30m\x1B[47m"
#define KGRY  "\x1B[30m\x1B[100m"
		"No Clouds    Partly Cloudy    Mostly Cloudy    Overcast\n"
		"             " KGRY "                 " KLGRY "                 " KWHT "        " KNRM "\n"
	      );
}


//TODO support looping
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
	while((opt=getopt(argc, argv, "lahkIm:t:x:y:w:h:c:r:i:")) != -1 )  {
		switch(opt) {
			case 'a':
				args->animated = 1;
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

// TODO top level error checking
int main(int argc, char **argv)
{
	struct map map;
	struct args args;
	int r = 0;
	char img_dir[]="/tmp/tmp.XXXXXX";
	char img_path[512];

	getOptions(&args, argc, argv);

	r = load_map(&map, args.map_file);
	map.image_type = args.image_type;

	if(args.test_image) {
		r = vt_one_hundrify(&map, args.test_image);
	}
	else {
		// Create dir for frames
    		mkdtemp(img_dir);
		// This will probably change with animation
		sprintf(img_path, "%s/0.png", img_dir);

		struct timeval tv;
		gettimeofday(&tv, NULL);
		unsigned long long now =
			(unsigned long long)(tv.tv_sec) * 1000 +
			(unsigned long long)(tv.tv_usec) / 1000;
		r = download_weather_image(&map, now, img_path);
		r = vt_one_hundrify(&map, img_path);
    		remove(img_path);

    		remove(img_dir);
	}

	map.panx = args.x;
	map.pany = args.y;
	map.renderw = args.w;
	map.renderh = args.h;

	if(args.interactive) {
		system("stty raw -echo");
		int key = 0;
		while (key != 'q') { // TODO also esc
			key = getchar();
			switch(key) {
				case 'k': // TODO also up arrow
					if (map.pany > 0)
						map.pany--;
					break;
				case 'j': // TODO also down arrow
					if (map.pany + map.renderh < map.h)
						map.pany++;
					break;
				case 'h': // TODO also left arrow
					if (map.panx > 0)
						map.panx--;
					break;
				case 'l': // TODO also right arrow
					if (map.panx + map.renderw < map.w)
						map.panx++;
					break;
			}
			print_map(&map, 1);
		}
		system("stty cooked echo");
	}
	else
		print_map(&map, 0);

	free_map(&map);

	return 0;
}
