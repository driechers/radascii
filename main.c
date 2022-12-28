#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "map.h"
#include "fetch.h"
#include "vt_one_hundrify.h"

//TODO support looping
//TODO support animation
struct args
{
	int loop;
	int animated;
	char *map_file;
	char *test_image;
	int x;
	int y;
	int w;
	int h;
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
		"\t-h Dispaly this menu.\n"
		"\t-a Play radar animation once.\n"
		"\t-l Continuously play radar loop.\n");
}

void getOptions(struct args *args, int argc, char **argv)
{
	int opt;

	// Defaults
	char *str_x = "0";
	char *str_y = "0";
	char *str_c = "80";
	char *str_r = "24";
	args->map_file = "usmca.txt";
	args->test_image = NULL;

	// Process Args
	while((opt=getopt(argc, argv, "lahm:t:x:y:w:h:c:r:")) != -1 )  {
		switch(opt) {
			case 'l':
				args->loop = 1;
				break;
			case 'a':
				args->animated = 1;
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

	if(args.test_image)
		r = vt_one_hundrify(&map, args.test_image);
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
		r = download_radar_image(&map, now, img_path);
		r = vt_one_hundrify(&map, img_path);
    		remove(img_path);

    		remove(img_dir);
	}

	map.panx = args.x;
	map.pany = args.y;
	map.renderw = args.w;
	map.renderh = args.h;
	print_map(&map);
	free_map(&map);

	return 0;
}
