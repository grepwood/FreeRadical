#include <stdio.h>
#include <stdint.h>
#include "dataio/BigEndian.h"

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

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
size_t FR_fread_b16(int16_t * dest, FILE * file) {
	size_t result = fread(dest,2,1,file);
	*dest = FR_bswap16(*dest);
	return result;
}
size_t FR_fread_b32(int32_t * dest, FILE * file) {
	size_t result = fread(dest,4,1,file);
	*dest = FR_bswap32(*dest);
	return result;
}
size_t FR_fread_b64(int64_t * dest, FILE * file) {
	size_t result = fread(dest,8,1,file);
	*dest = FR_bswap64(*dest);
	return result;
}
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
size_t FR_fread_l16(int16_t * dest, FILE * file) {
	size_t result = fread(dest,2,1,file);
	*dest = FR_bswap16(*dest);
	return result;
}
size_t FR_fread_l32(int32_t * dest, FILE * file) {
	size_t result = fread(dest,4,1,file);
	*dest = FR_bswap32(*dest);
	return result;
}
size_t FR_fread_l64(int64_t * dest, FILE * file) {
	size_t result = fread(dest,8,1,file);
	*dest = FR_bswap64(*dest);
	return result;
}
#endif
