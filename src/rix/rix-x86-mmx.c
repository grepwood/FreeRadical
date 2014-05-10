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
		uint64_t Q[96];
	} RIXPalette;
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
	Index.q = 0x0004000400040004;
	__asm__("movq mm7, %0\n" : "=m"(Index.q)); /* We're initializing our multipliers */
	fseek(RIX,10,SEEK_SET);
	fread(RIXPalette.B,768,1,RIX);
	for(Counter = 0; Counter < 96; ++Counter)
	{
		__asm__("movq mm0, %0\n"
				"pmullw mm0, mm7\n"
				"movq %0, mm0\n" : "=m"(RIXPalette.Q[Counter]));
	}
	Index.q = 0x0003000300030003;
	__asm__("movq mm7, %0\n" : "=m"(Index.q)); /* We're initializing our multipliers */
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
			BGR.b[0] = RIXPalette.B[Index.w[0]+2];
			BGR.b[1] = RIXPalette.B[Index.w[0]+1];
			BGR.b[2] = RIXPalette.B[Index.w[0]];
			BGR.b[3] = RIXPalette.B[Index.w[1]+2];
			BGR.b[4] = RIXPalette.B[Index.w[1]+1];
			BGR.b[5] = RIXPalette.B[Index.w[1]];
			BGR.b[6] = RIXPalette.B[Index.w[2]+2];
			BGR.b[7] = RIXPalette.B[Index.w[2]+1];
			BGR.b[8] = RIXPalette.B[Index.w[2]];
			BGR.b[9] = RIXPalette.B[Index.w[3]+2];
			BGR.b[10] = RIXPalette.B[Index.w[3]+1];
			BGR.b[11] = RIXPalette.B[Index.w[3]];
			Index.w[0] = fgetc(RIX);
			Index.w[1] = fgetc(RIX);
			Index.w[2] = fgetc(RIX);
			Index.w[3] = fgetc(RIX);
			__asm__("movq mm0, %0\n"
					"pmullw mm0, mm7\n"
					"movq %0, mm0\n" : "=m"(Index.q));
			BGR.b[12] = RIXPalette.B[Index.w[0]+2];
			BGR.b[13] = RIXPalette.B[Index.w[0]+1];
			BGR.b[14] = RIXPalette.B[Index.w[0]];
			BGR.b[15] = RIXPalette.B[Index.w[1]+2];
			BGR.b[16] = RIXPalette.B[Index.w[1]+1];
			BGR.b[17] = RIXPalette.B[Index.w[1]];
			BGR.b[18] = RIXPalette.B[Index.w[2]+2];
			BGR.b[19] = RIXPalette.B[Index.w[2]+1];
			BGR.b[20] = RIXPalette.B[Index.w[2]];
			BGR.b[21] = RIXPalette.B[Index.w[3]+2];
			BGR.b[22] = RIXPalette.B[Index.w[3]+1];
			BGR.b[23] = RIXPalette.B[Index.w[3]];
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