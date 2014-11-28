#include <stdio.h>
#if _FILE_OFFSET_BITS < 64
#	unfedine _FILE_OFFSET_BITS
#	define _FILE_OFFSET_BITS 64
#ifdef
char * FR_textline(FILE * stream) {
	char * result = NULL;
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
		fread(result,len,1,stream);
		result[len]=0;
	}
	return result;
}
