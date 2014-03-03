#include <stdint.h>

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
	return foo.c[1];
}
