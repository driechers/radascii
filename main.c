#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include "map.h"
#include "fetch.h"
#include "vt_one_hundrify.h"


void usage()
{
    printf("Usage:\n\n"
           "radascii [options]\n"
           "\tDefault behavior is to display the most recent radar image over Us Mexico Canida map.\n"
	   "\t-t <test radar png file>\n"
	   "\t-m <ascii map file>\n"
	   "\t-x <x coord to pan to>\n"
	   "\t-y <y coord to pan to>\n"
           "\t-h Dispaly this menu.\n"
           "\t-a Play radar animation once.\n"
           "\t-l Continuously play radar loop.\n");
}

// TODO support panning
// TODO support console size
void getOptions(int *loop, int *animated, char **map_file, char **test_image, int *x, int *y, int argc, char **argv)
{
	int opt;

	// Defaults
	char *str_x = "0";
	char *str_y = "0";
	*map_file = "usmca.txt";
	*test_image = NULL;

	// Process Args
	while((opt=getopt(argc, argv, "lahm:t:x:y:")) != -1 )  {
		switch(opt) {
			case 'l':
				*loop = 1;
				break;
			case 'a':
				*animated = 1;
				break;
			case 'h':
				usage();
				exit(0);
			case 'm':
				*map_file = optarg;
				break;
			case 't':
				*test_image = optarg;
				break;
			case 'x':
				str_x = optarg;
				break;
			case 'y':
				str_y = optarg;
				break;
			default:
				usage();
				exit(-1);
		}
	}

	// Convert args
	*x = atoi(str_x);
	*y = atoi(str_y);
}


// TODO top level error checking
int main(int argc, char **argv)
{
	struct map map;
	int r = 0;
	//TODO support looping
	int loop = 0;
	//TODO support animation
	int animated = 0;
	int pan_x = 0;
	int pan_y = 0;
	char *map_file = NULL;
	char *test_image = NULL;
	char img_dir[]="/tmp/tmp.XXXXXX";
	char img_path[512];

	getOptions(&loop, &animated, &map_file, &test_image, &pan_x, &pan_y, argc, argv);
	printf("%d %d\n", pan_x, pan_y);

	r = load_map(&map, "usmca.txt");

	if(test_image)
		r = vt_one_hundrify(&map, "test.png");
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

	map.panx = pan_x;
	map.pany = pan_y;
	//map.panx = 200;
	//map.pany = 0;
	print_map(&map);
	free_map(&map);

	return 0;
}
