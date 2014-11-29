#include <stdio.h>
#if _FILE_OFFSET_BITS < 64
#	error "File offsets must be 64bit"
#endif
char * FR_textline(FILE * stream);
