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

char IsFalloutRIX(FILE * Splash) /* File must be set on beginning */
{
	char result = 0;
	size_t ActualSize = 0;
	struct RIXheader RIX = {{0,0,0,0},0,0,0};
	fread(&RIX.Signature,4,1,Splash);
	if(!BigEndian())
	{
		fread(&RIX.Width,2,1,Splash);
		fread(&RIX.Height,2,1,Splash);
		fread(&RIX.Unknown,2,1,Splash);
	}
	else
	{
		RIX.Width = ReadAlienEndian16(Splash);
		RIX.Height = ReadAlienEndian16(Splash);
		RIX.Unknown = ReadAlienEndian16(Splash);
	}
	fseeko(Splash,0,SEEK_END);
	ActualSize = ftello(Splash);
	fseeko(Splash,0,SEEK_SET);
	if(!strncmp(RIX.Signature,"RIX3",4))
	{
		result += 1;
	}
	if(ActualSize == 307978)
	{
		result += 2;
	}
	if(RIX.Width == 640)
	{
		result += 4;
	}
	if(RIX.Height == 480)
	{
		result += 8;
	}
	if(RIX.Unknown == 0x00AF)
	{
		result += 16;
	}
	return result;
}

char * FalloutRIX2BMPMem(FILE * RIX)
{
	union
	{
		char B[768];
		__m128 QW[48];
	} RIXPalette;
	char * bitmap = malloc(921654);
	int32_t Position;
	uint16_t Counter;
	uint16_t Index[8] = {4,4,4,4,4,4,4,4}; /* Palette multiplier */
	char * BGR; /* Helper pointer */
/* Simple BMP header for a 640x480x24 pic */
	char bmpheader[54] = {	66,77,54,16,14,0,0,0,0,0,54,0,0,0,40,0,0,0,128,2,0,0,224,1,0,0,
				1,0,24,0,0,0,0,0,0,16,14,0,19,11,0,0,19,11,0,0,0,0,0,0,0,0,0,0};
	memcpy(bitmap,bmpheader,54);
	__asm__("movaps xmm7, %0\n" : "=m"(Index));
	fseek(RIX,10,SEEK_SET);
	fread(RIXPalette.B,768,1,RIX);
/* We're going to saturate our RIX palette because this is better to do */
	for(Counter = 0; Counter < 48; ++Counter)
	{
		__asm__("movaps xmm0, %0\n"
				"pmullw xmm0, xmm7\n"
				"movaps %0, xmm0\n" : "=m"(RIXPalette.QW[Counter]));
	}
/* Assigning index multiplier */
	Index[0] = 3;
	Index[1] = 3;
	Index[2] = 3;
	Index[3] = 3;
	Index[4] = 3;
	Index[5] = 3;
	Index[6] = 3;
	Index[7] = 3;
	__asm__("movaps xmm7, %0\n" : "=m"(Index));
	for(Position = 919734; Position >= 54; Position -= 1920)
	{
		for(Counter = 0; Counter < 1920; Counter += 48)
		{
			Index[0] = fgetc(RIX);
			Index[1] = fgetc(RIX);
			Index[2] = fgetc(RIX);
			Index[3] = fgetc(RIX);
			Index[4] = fgetc(RIX);
			Index[5] = fgetc(RIX);
			Index[6] = fgetc(RIX);
			Index[7] = fgetc(RIX);
			__asm__("movaps xmm0, %0\n"
					"pmullw xmm0, xmm7\n"
					"movaps %0, xmm0\n" : "=m"(Index));
			BGR = bitmap+Position+Counter;
			BGR[0] = RIXPalette.B[Index[0]+2];
			BGR[1] = RIXPalette.B[Index[0]+1];
			BGR[2] = RIXPalette.B[Index[0]];
			BGR[3] = RIXPalette.B[Index[1]+2];
			BGR[4] = RIXPalette.B[Index[1]+1];
			BGR[5] = RIXPalette.B[Index[1]];
			BGR[6] = RIXPalette.B[Index[2]+2];
			BGR[7] = RIXPalette.B[Index[2]+1];
			BGR[8] = RIXPalette.B[Index[2]];
			BGR[9] = RIXPalette.B[Index[3]+2];
			BGR[10] = RIXPalette.B[Index[3]+1];
			BGR[11] = RIXPalette.B[Index[3]];
			BGR[12] = RIXPalette.B[Index[4]+2];
			BGR[13] = RIXPalette.B[Index[4]+1];
			BGR[14] = RIXPalette.B[Index[4]];
			BGR[15] = RIXPalette.B[Index[5]+2];
			BGR[16] = RIXPalette.B[Index[5]+1];
			BGR[17] = RIXPalette.B[Index[5]];
			BGR[18] = RIXPalette.B[Index[6]+2];
			BGR[19] = RIXPalette.B[Index[6]+1];
			BGR[20] = RIXPalette.B[Index[6]];
			BGR[21] = RIXPalette.B[Index[7]+2];
			BGR[22] = RIXPalette.B[Index[7]+1];
			BGR[23] = RIXPalette.B[Index[7]];
			Index[0] = fgetc(RIX);
			Index[1] = fgetc(RIX);
			Index[2] = fgetc(RIX);
			Index[3] = fgetc(RIX);
			Index[4] = fgetc(RIX);
			Index[5] = fgetc(RIX);
			Index[6] = fgetc(RIX);
			Index[7] = fgetc(RIX);
			__asm__("movaps xmm0, %0\n"
					"pmullw xmm0, xmm7\n"
					"movaps %0, xmm0\n" : "=m"(Index));
			BGR[24] = RIXPalette.B[Index[0]+2];
			BGR[25] = RIXPalette.B[Index[0]+1];
			BGR[26] = RIXPalette.B[Index[0]];
			BGR[27] = RIXPalette.B[Index[1]+2];
			BGR[28] = RIXPalette.B[Index[1]+1];
			BGR[29] = RIXPalette.B[Index[1]];
			BGR[30] = RIXPalette.B[Index[2]+2];
			BGR[31] = RIXPalette.B[Index[2]+1];
			BGR[32] = RIXPalette.B[Index[2]];
			BGR[33] = RIXPalette.B[Index[3]+2];
			BGR[34] = RIXPalette.B[Index[3]+1];
			BGR[35] = RIXPalette.B[Index[3]];
			BGR[36] = RIXPalette.B[Index[4]+2];
			BGR[37] = RIXPalette.B[Index[4]+1];
			BGR[38] = RIXPalette.B[Index[4]];
			BGR[39] = RIXPalette.B[Index[5]+2];
			BGR[40] = RIXPalette.B[Index[5]+1];
			BGR[41] = RIXPalette.B[Index[5]];
			BGR[42] = RIXPalette.B[Index[6]+2];
			BGR[43] = RIXPalette.B[Index[6]+1];
			BGR[44] = RIXPalette.B[Index[6]];
			BGR[45] = RIXPalette.B[Index[7]+2];
			BGR[46] = RIXPalette.B[Index[7]+1];
			BGR[47] = RIXPalette.B[Index[7]];
		}
	}
	return bitmap;
}

void logSDLError(char * os, const char * msg){
	printf("%s%s error: %s\n",os,msg,SDL_GetError());
}

int main(int argc, char * argv[])
{
	FILE * fp = NULL;
	int result = 0;
	char * bitmap = NULL;
	/* SDL stuff */
	SDL_Window * win = NULL;
	SDL_Renderer * ren = NULL;
	SDL_Surface * bmp = NULL;
	SDL_Texture * tex = NULL;
	SDL_RWops * rixmap = NULL;
	/* Declarations end */
	if(argc != 2){ exit(1); }
	fp = fopen(argv[1],"rb");
	if(IsFalloutRIX(fp) == 31)
	{
		/* Preparing */
		if(SDL_Init(SDL_INIT_EVERYTHING))
		{
			printf("SDL_Init Error: %s\n",SDL_GetError());
			result += 1;
		}
		win = SDL_CreateWindow("Hello RIX3!",100,100,640,480,SDL_WINDOW_SHOWN);
		if(win == NULL)
		{
			printf("SDL_CreateWindow Error: %s\n",SDL_GetError());
			result += 2;
		}
		ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(ren == NULL)
		{
			printf("SDL_CreateRenderer Error: %s\n",SDL_GetError());
			result += 4;
		}
		bitmap = FalloutRIX2BMPMem(fp);
		rixmap = SDL_RWFromConstMem(bitmap,921654);
		bmp = SDL_LoadBMP_RW(rixmap,1);
		if(bmp == NULL)
		{
			printf("SDL_LoadBMP Error: %s\n",SDL_GetError());
			result += 8;
		}
		tex = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_FreeSurface(bmp);
		if (tex == NULL)
		{
			printf("SDL_CreateTextureFromSurface Error: %s\n",SDL_GetError());
			result += 16;
		}
		/* Drawing */
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
		SDL_Delay(2000);
		/* Cleaning up */
		SDL_DestroyTexture(tex);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		free(bitmap);
	}
	else
	{
		result += 32;
	}
	fclose(fp);
	return result;
}
