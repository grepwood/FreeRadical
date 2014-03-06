#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <png.h>

#include <rix/rix.h>
#include <dataio/BigEndian.h>

/* Returns 0b00011111 subtract failed criterions
 * Signature is 0th bit
 * File size is 1st bit
 * Width is 2nd bit
 * Height is 3rd bit
 * Control is 4th bit
 * We lack use for 5th, 6th and 7th bit
 * Consider 31 a 100% match */

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
		result += 0b00000001;
	}
	if(ActualSize == 307978)
	{
		result += 0b00000010;
	}
	if(RIX.Width == 640)
	{
		result += 0b00000100;
	}
	if(RIX.Height == 480)
	{
		result += 0b00001000;
	}
	if(RIX.Unknown == 0x00AF)
	{
		result += 0b00010000;
	}
	return result;
}



/* This function works on the assumption that IsFalloutRIX returned 31 */

void FalloutRIX2BMPFile(FILE * RIX, FILE * bmp)
{
	char RIXPalette[768];
	char bitmap[921600];	/* This is where we store all 640x480x24 pixels */
	char OrderedPixel[3];	/* We need this because BMP stores in BGR while RIX stores in RGB */
	int32_t Position;
	uint16_t Counter;
	uint16_t Index;
/* Simple BMP header for a 640x480x24 pic */
	char bmpheader[54] = {	0x42,0x4D,0x36,0x10,0x0E,0x00,0x00,0x00,
							0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
							0x00,0x00,0x80,0x02,0x00,0x00,0xE0,0x01,
							0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
							0x00,0x00,0x00,0x10,0x0E,0x00,0x13,0x0B,
							0x00,0x00,0x13,0x0B,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00};
	fwrite(bmpheader,54,1,bmp);
	fseeko(RIX,10,SEEK_SET);
	fread(RIXPalette,768,1,RIX);
	for(Position = 919680; Position >= 0; Position -= 1920)
	{
		for(Counter = 0; Counter < 1920; Counter += 3)
		{
			Index = 3*fgetc(RIX);
			OrderedPixel[0] = *(RIXPalette+Index+2);
			OrderedPixel[1] = *(RIXPalette+Index+1);
			OrderedPixel[2] = *(RIXPalette+Index);
			memcpy(bitmap+Position+Counter,OrderedPixel,3);
		}
	}
	fwrite(bitmap,921654,1,bmp);
}

char * FalloutRIX2BMPMem(FILE * RIX)
{
	char RIXPalette[768];
	char * bitmap = malloc(921654);
	char OrderedPixel[3];
	int32_t Position;
	uint16_t Counter;
	uint16_t Index;
/* Simple BMP header for a 640x480x24 pic */
	char bmpheader[54] = {	0x42,0x4D,0x36,0x10,0x0E,0x00,0x00,0x00,
							0x00,0x00,0x36,0x00,0x00,0x00,0x28,0x00,
							0x00,0x00,0x80,0x02,0x00,0x00,0xE0,0x01,
							0x00,0x00,0x01,0x00,0x18,0x00,0x00,0x00,
							0x00,0x00,0x00,0x10,0x0E,0x00,0x13,0x0B,
							0x00,0x00,0x13,0x0B,0x00,0x00,0x00,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00};
	memcpy(bitmap,bmpheader,54);
	fseeko(RIX,10,SEEK_SET);
	fread(RIXPalette,768,1,RIX);
	for(Position = 919734; Position >= 54; Position -= 1920)
	{
		for(Counter = 0; Counter < 1920; Counter += 3)
		{
			Index = 3*fgetc(RIX);
			OrderedPixel[0] = *(RIXPalette+Index+2);
			OrderedPixel[1] = *(RIXPalette+Index+1);
			OrderedPixel[2] = *(RIXPalette+Index);
			memcpy(bitmap+Position+Counter,OrderedPixel,3);
		}
	}
	return bitmap;
}

int main(int argc, char * argv[])
{
	FILE * fp = NULL;
	FILE * bmp = NULL;
	char * bitmap = NULL;
	if(argc != 3){ exit(1); }
	fp = fopen(argv[1],"rb");
	bmp = fopen(argv[2],"wb");
//	FalloutRIX2bmp(fp,bmp);
	bitmap = FalloutRIX2BMPMem(fp);
	fwrite(bitmap,921654,1,bmp);
	free(bitmap);
	fclose(fp);
	fclose(bmp);
	return 0;
}
