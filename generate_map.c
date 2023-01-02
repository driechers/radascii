#include <stdio.h>

// Each 3x3 image represents the ascii char listed
// Assume that images perform extra erossion.
// In other words account for cases where pixels are missing,
// but do not account for cases of extra pixels
// 10 3x3 images for all characters

struct block {
	int populated;
	int block[3][3];
};

static struct block char_blocks [256][10] =
{
	['\\'] = {
	  {1, {{1, 0, 0},
	       {0, 1, 0},
	       {0, 0, 1}}},
	  {1, {{1, 0, 0},
	       {0, 0, 0},
	       {0, 0, 1}}}
	},
	['/'] = {
	  {1, {{0, 0, 1},
	       {0, 1, 0},
	       {1, 0, 0}}},
	  {1, {{0, 0, 1},
	       {0, 0, 0},
	       {1, 0, 0}}}
	},
};

int main(void)
{
	for (int c = 0; c < 256; c++) {
		if (char_blocks[c][0].populated) {
			printf("%c :\n", c);
			for (int b = 0; b < 10; b++) {
				if (char_blocks[c][b].populated) {
					for (int i=0; i < 3; i++) {
						for (int j=0; j < 3; j++) {
							printf("\t%d", char_blocks[c][b].block[i][j]);
						}
						printf("\n");
					}
					printf("\n");
				}
			}
		}
	}

	return 0;
}