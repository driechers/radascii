#ifndef VT_ONE_HUNDRIFY_H
#define VT_ONE_HUNDRIFY_H

#include "map.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[41m"
#define KGRN  "\x1B[42m"
#define KYEL  "\x1B[43m"
#define KBLU  "\x1B[44m"
#define KMAG  "\x1B[45m"
#define KCYN  "\x1B[46m"
#define KWHT  "\x1B[30m\x1B[107m"
#define KLGRY "\x1B[30m\x1B[47m"
#define KGRY  "\x1B[30m\x1B[100m"
#define KCLFT "\x1B[80C"
#define KCUP  "\x1B[26A"
#define KCDN  "\x1B[26B"
#define KCHM  "\x1B[H"
#define KCHD  "\x1B[?25l"
#define KCSW  "\x1B[?25h"

void libpng_init (void);
int vt_one_hundrify(struct map *map, char *filename);
const char* color_to_vt100(enum color color);

#endif
