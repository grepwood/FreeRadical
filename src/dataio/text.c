#include <stdio.h>
#include <stdlib.h>
#include "dataio/dat.h"
#if _FILE_OFFSET_BITS < 64
#	error "File offsets have to be 64bit"
#endif
char * FR_textline(FILE * stream) {
	char * result = NULL;
	int e;
	size_t len = 0;
	size_t offset = ftello(stream);
	char buf;
	do {
		buf = fgetc(stream);
		if(buf) ++len;
	} while(buf);
	fseeko(stream,offset,SEEK_SET);
	result = malloc(len+1);
	if(result) {
		e = fread(result,len,1,stream);
		if(!e) fputs("FR_textline couldn't read string when previously it could!\n",stderr);
		result[len]=0;
	}
	return result;
}
