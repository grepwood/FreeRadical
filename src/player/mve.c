#include <stdio.h>
#include <string.h>

/* Will return 0 (false) if file is MVE */
int FileIsNotMVE(FILE * Movie)
{
	char Control[20];
	fseeko(Movie,0,SEEK_SET);
	fread(Control,20,1,Movie);
	fseeko(Movie,0,SEEK_SET);
	return memcmp(Control,"Interplay MVE File\x1A\0",20);
}
