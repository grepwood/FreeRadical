#ifndef _GNU_SOURCE
#	define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <SDL2/SDL.h>
#include <rix/rix.h>
#include <dataio/BigEndian.h>

unsigned char bmpheader[54] = {	66,77,54,16,14,0,0,0,0,0,54,0,0,0,40,0,0,0,128,2,0,0,224,1,0,0,
				1,0,24,0,0,0,0,0,0,16,14,0,19,11,0,0,19,11,0,0,0,0,0,0,0,0,0,0};

/* Returns 0b00111111 subtract failed criterions
 * Signature is 0th bit
 * File size is 1st bit
 * Width is 2nd bit
 * Height is 3rd bit
 * Palette type is 4th bit
 * Storage type is 5th bit
 * We lack use for 6th and 7th bit
 * Consider 63 a 100% match */

char IsFalloutRIX(FILE * Splash) { /* File must be set on beginning */
	char result = 0;
	static size_t ActualSize;
	static struct RIXheader RIX;
	fread(&RIX.Signature,4,1,Splash);
	fread(&RIX.Width,2,1,Splash);
	fread(&RIX.Height,2,1,Splash);
	RIX.PT = (char)fgetc(Splash);
	RIX.ST = (char)fgetc(Splash);
	if(BigEndian()) {
		RIX.Width = FR_bswap16(RIX.Width);
		RIX.Height = FR_bswap16(RIX.Height);
	}
	fseeko(Splash,0,SEEK_END);
	ActualSize = ftello(Splash);
	fseeko(Splash,10,SEEK_SET);
	if(!memcmp(RIX.Signature,"RIX3",4)) {
		result += 1;
	}
	if(ActualSize == 307978) {
		result += 2;
	}
	if(RIX.Width == 640) {
		result += 4;
	}
	if(RIX.Height == 480) {
		result += 8;
	}
	if(RIX.PT == 0xAF) { /* VGA */
		result += 16;
	}
	if(!RIX.ST) { /* Linear */
		result += 32;
	}
	return result;
}

unsigned char * FalloutRIX2BMPMem(FILE * RIX) {
	static unsigned char RIXPalette[768];
	static size_t * palptr = (size_t *)RIXPalette;
	unsigned char * bitmap = malloc(921654);
	static unsigned char tmp;
	static uint16_t Index = sizeof(size_t);
	int32_t Position = 768/Index;
	static uint16_t Counter;
/* Simple BMP header for a 640x480x24 pic */
	memcpy(bitmap,bmpheader,54);
	fread(RIXPalette,768,1,RIX);
	for(Counter = 0; Counter < Position; ++Counter) {
		palptr[Counter] <<= 2; /* Since each byte is less than 64, this will never cause an overflow */
	}
	for(Counter = 0; Counter < 768; Counter += 3) {
		tmp = RIXPalette[Counter];
		RIXPalette[Counter] = RIXPalette[Counter+2];
		RIXPalette[Counter+2] = tmp;
	}
	for(Position = 919734; Position >= 54; Position -= 1920) {
		for(Counter = 0; Counter < 1920; Counter += 3) {
			Index = 3*fgetc(RIX);
			memcpy(bitmap+Position+Counter,RIXPalette+Index,3);
		}
	}
	return bitmap;
}
