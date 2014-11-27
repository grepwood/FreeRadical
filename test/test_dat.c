#include "dataio/dat.h"
#include <stdlib.h>

/* Run this program through a debugger.
 * It shouldn't segfault, and here are recorded heap results:
 * Fallout 1 CRITTER.DAT:	291KB
 * Fallout 2 MASTER.DAT:	1MB
 */

int main(int argc, char **argv) {
	struct fr_dat_handler_t okay;
	if(argc != 2) exit(1);
	FR_OpenDAT(argv[1],&okay);
	FR_ReadDAT(&okay);
	FR_CloseDAT(&okay);
	return 0;
}
