#include <stdint.h>

/* Note: all values in a RIX3 file are little endian */

unsigned char * FalloutRIX2BMPMem(FILE * RIX);
char IsFalloutRIX(FILE * Splash);

struct RIXheader {
	char Signature[4];	/* file signature (RIX3 in ASCII) */
	uint16_t Width; 	/* width of the image (640 pixels, 0x0280) */
	uint16_t Height; 	/* height of the image (480 pixels, 0x01E0) */
	uint8_t PT;			/* Palette type. 0xAF specifies VGA and is the expected value. */
	uint8_t ST;			/* Storage type. 0x00 specifies linear storage. */
};
