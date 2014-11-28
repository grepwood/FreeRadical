#include <stdio.h>
#if _FILE_OFFSET_BITS < 64
#	unfedine _FILE_OFFSET_BITS
#	define _FILE_OFFSET_BITS 64
#ifdef
char * FR_textline(FILE * stream);
