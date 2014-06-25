#include <stdio.h>

/* This is how the engine deals with endianesses different than the host's */

/* We use a function to determine endianess of a host because there
 * exist such freaks of nature like bi-endian machines and endian
 * switching. */

char BigEndian(void)
{
	union {
		short int w;
		char b[2];
	} foo = {0x0100};
	return foo.b[0];
}
