#include <stdio.h>

#include "map.h"
#include "vt_one_hundrify.h"

int main(void)
{
	struct map map;
	int r = load_map(&map, "usmca.txt");
	//printf("%d\n", r);
	r = vt_one_hundrify(&map, "test.png");
	printf("ret %d\n", r);
	map.panx = 60;
	map.pany = 0;
	map.panx = 200;
	map.pany = 0;
	print_map(&map);
	free_map(&map);
	//printf("%s\n", KRED "RED " KGRN "GREEN " KYEL "YELLOW" KBLU "BLUE" KMAG "MAGENTA" KCYN "CYAN" KNRM);
	return 0;
}
