#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char * FalloutRIX2BMPMem(FILE * RIX)
{
	char RIXPalette[768];
	char * bitmap = malloc(921654);
	int32_t Position;
	uint16_t Counter;
	union
	{
		uint16_t w[4];
		uint64_t q;
	} Index;
	union
	{
		char * b;
		uint64_t * q;
	} BGR;
/* Simple BMP header for a 640x480x24 pic */
	char bmpheader[54] = {	66,77,54,16,14,0,0,0,0,0,54,0,0,0,40,0,0,0,128,2,0,0,224,1,0,0,
				1,0,24,0,0,0,0,0,0,16,14,0,19,11,0,0,19,11,0,0,0,0,0,0,0,0,0,0};
	memcpy(bitmap,bmpheader,54);
	Index.q = 0x0003000300030003;
	__asm__("movq mm7, %0\n" : "=m"(Index.q));
	Index.q = 0x0004000400040004;
	__asm__("movq mm6, %0\n" : "=m"(Index.q)); /* We're initializing our multipliers */
	fseek(RIX,10,SEEK_SET);
	fread(RIXPalette,768,1,RIX);
	for(Position = 919734; Position >= 54; Position -= 1920)
	{
		for(Counter = 0; Counter < 1920; Counter += 24)
		{
			Index.w[0] = fgetc(RIX);
			Index.w[1] = fgetc(RIX);
			Index.w[2] = fgetc(RIX);
			Index.w[3] = fgetc(RIX);
			__asm__("movq mm0, %0\n"
					"pmullw mm0, mm7\n"
					"movq %0, mm0\n" : "=m"(Index.q));
			BGR.b = bitmap+Position+Counter;
			BGR.b[0] = RIXPalette[Index.w[0]+2];
			BGR.b[1] = RIXPalette[Index.w[0]+1];
			BGR.b[2] = RIXPalette[Index.w[0]];
			BGR.b[3] = RIXPalette[Index.w[1]+2];
			BGR.b[4] = RIXPalette[Index.w[1]+1];
			BGR.b[5] = RIXPalette[Index.w[1]];
			BGR.b[6] = RIXPalette[Index.w[2]+2];
			BGR.b[7] = RIXPalette[Index.w[2]+1];
			BGR.b[8] = RIXPalette[Index.w[2]];
			BGR.b[9] = RIXPalette[Index.w[3]+2];
			BGR.b[10] = RIXPalette[Index.w[3]+1];
			BGR.b[11] = RIXPalette[Index.w[3]];
			Index.w[0] = fgetc(RIX);
			Index.w[1] = fgetc(RIX);
			Index.w[2] = fgetc(RIX);
			Index.w[3] = fgetc(RIX);
			__asm__("movq mm0, %0\n"
					"pmullw mm0, mm7\n"
					"movq %0, mm0\n" : "=m"(Index.q));
			BGR.b[12] = RIXPalette[Index.w[0]+2];
			BGR.b[13] = RIXPalette[Index.w[0]+1];
			BGR.b[14] = RIXPalette[Index.w[0]];
			BGR.b[15] = RIXPalette[Index.w[1]+2];
			BGR.b[16] = RIXPalette[Index.w[1]+1];
			BGR.b[17] = RIXPalette[Index.w[1]];
			BGR.b[18] = RIXPalette[Index.w[2]+2];
			BGR.b[19] = RIXPalette[Index.w[2]+1];
			BGR.b[20] = RIXPalette[Index.w[2]];
			BGR.b[21] = RIXPalette[Index.w[3]+2];
			BGR.b[22] = RIXPalette[Index.w[3]+1];
			BGR.b[23] = RIXPalette[Index.w[3]];
			__asm__("movq mm2, %0\n"
					"pmullw mm2, mm6\n"
					"movq %0, mm2\n" : "=m"(BGR.q[0]));
			__asm__("movq mm3, %0\n"
					"pmullw mm3, mm6\n"
					"movq %0, mm3\n" : "=m"(BGR.q[1]));
			__asm__("movq mm4, %0\n"
					"pmullw mm4, mm6\n"
					"movq %0, mm4\n" : "=m"(BGR.q[2]));
		}
	}
	return bitmap;
}
