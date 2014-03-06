#include <stdint.h>

/* Note: all values in a RIX3 file are little endian */

struct RIXheader
{
	char Signature[4];	/* file signature (RIX3 in ASCII) */
	uint16_t Width; 	/* width of the image (640 pixels, 0x0280) */
	uint16_t Height; 	/* height of the image (480 pixels, 0x01E0) */
	uint16_t Unknown;	/* Unknown. Possibly the number of colors used.
						 * Fallout 2 uses only one value (0x00AF) in the RIX file header.
						 * Suppose we can use it as a control value? */
};

struct RGB
{
	unsigned char Red;		/* Red intensity */
	unsigned char Green;	/* Green intensity */
	unsigned char Blue;		/* Blue intensity */
};

struct Palette
{
	char entries[768]; /* 3*256 */
};
