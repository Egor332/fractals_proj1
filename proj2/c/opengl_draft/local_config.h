#ifndef LOCAL_CONFIG_H
#define LOCAL_CONFIG_H
#include <global_config.h>

/* nr of samples */
#define PERC_MAX_LVL 500

/* finish when dull */
#define PERC_FINISH_RATIO_NUM 3
#define PERC_FINISH_RATIO_DEN 5

/* quality */
#define CELL_SIZE 4
#define WORLD_WIDTH (SCR_WIDTH/CELL_SIZE)
#define WORLD_HEIGHT (SCR_HEIGHT/CELL_SIZE)

#endif
