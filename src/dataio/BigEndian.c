#include <stdint.h>

/* Generic function for reading a DWORD off an alien endian */

uint32_t ReadAlienEndian(FILE * gah)
{
        union
        {
                uint32_t result;
                char c[4];
        }
        foo = {0x00000000};
        foo.c[3] = fgetc(gah);
        foo.c[2] = fgetc(gah);
        foo.c[1] = fgetc(gah);
        foo.c[0] = fgetc(gah);
        return foo.result;
}

/* We use a function to determine endianess of a host because there
 * exist such freaks of nature like bi-endian machines and endian
 * switching. */

char BigEndian(void)
{
	union
	{
		uint16_t i;
		char c[2];
	} foo = {0x0100};
	return foo.c[0];
}
