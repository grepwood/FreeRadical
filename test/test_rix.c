#include <stdio.h>
#include <SDL2/SDL.h>

#include <rix/rix.h>

void logSDLError(char * os, const char * msg){
	printf("%s%s error: %s\n",os,msg,SDL_GetError());
}

int main(int argc, char * argv[]) {
	FILE * fp = NULL;
	int result = 0;
	unsigned char * bitmap = NULL;
	char RIX_error;
	/* SDL stuff */
	SDL_Window * win = NULL;
	SDL_Renderer * ren = NULL;
	SDL_Surface * bmp = NULL;
	SDL_Texture * tex = NULL;
	SDL_RWops * rixmap = NULL;
	/* Declarations end */
	if(argc != 2) {
		puts("This test program requires a RIX file as an argument.");
		exit(64);
	}
	fp = fopen(argv[1],"rb");
	if(fp != NULL) {
		RIX_error = IsFalloutRIX(fp);
	}
	if(RIX_error == 63) {
		/* Preparing */
		if(SDL_Init(SDL_INIT_EVERYTHING)) {
			printf("SDL_Init Error: %s\n",SDL_GetError());
			result += 1;
		}
		win = SDL_CreateWindow("Hello RIX3!",100,100,640,480,SDL_WINDOW_SHOWN);
		if(win == NULL) {
			printf("SDL_CreateWindow Error: %s\n",SDL_GetError());
			result += 2;
		}
		ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if(ren == NULL) {
			printf("SDL_CreateRenderer Error: %s\n",SDL_GetError());
			result += 4;
		}
		bitmap = FalloutRIX2BMPMem(fp);
		rixmap = SDL_RWFromConstMem(bitmap,921654);
		bmp = SDL_LoadBMP_RW(rixmap,1);
		if(bmp == NULL) {
			printf("SDL_LoadBMP_RW Error: %s\n",SDL_GetError());
			result += 8;
		}
		tex = SDL_CreateTextureFromSurface(ren, bmp);
		SDL_FreeSurface(bmp);
		if (tex == NULL) {
			printf("SDL_CreateTextureFromSurface Error: %s\n",SDL_GetError());
			result += 16;
		}
		/* Drawing */
		SDL_RenderClear(ren);
		SDL_RenderCopy(ren, tex, NULL, NULL);
		SDL_RenderPresent(ren);
		SDL_Delay(20000);
		/* Cleaning up */
		SDL_DestroyTexture(tex);
		SDL_DestroyRenderer(ren);
		SDL_DestroyWindow(win);
		SDL_Quit();
		free(bitmap);
	}
	else {
		result += 32;
	}
	fclose(fp);
	return result;
}
