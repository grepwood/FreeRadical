#include <stdio.h>
#include <stdint.h>
#include "dataio/endian.h"

/* This is how the engine deals with endianesses different than the host's */

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
size_t FR_fread_b16(uint16_t * dest, FILE * file) {
	size_t result = fread(dest,2,1,file);
	*dest = FR_bswap16(*dest);
	return result;
}
size_t FR_fread_b32(uint32_t * dest, FILE * file) {
	size_t result = fread(dest,4,1,file);
	*dest = FR_bswap32(*dest);
	return result;
}
size_t FR_fread_b64(uint64_t * dest, FILE * file) {
	size_t result = fread(dest,8,1,file);
	*dest = FR_bswap64(*dest);
	return result;
}
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
size_t FR_fread_l16(uint16_t * dest, FILE * file) {
	size_t result = fread(dest,2,1,file);
	*dest = FR_bswap16(*dest);
	return result;
}
size_t FR_fread_l32(uint32_t * dest, FILE * file) {
	size_t result = fread(dest,4,1,file);
	*dest = FR_bswap32(*dest);
	return result;
}
size_t FR_fread_l64(uint64_t * dest, FILE * file) {
	size_t result = fread(dest,8,1,file);
	*dest = FR_bswap64(*dest);
	return result;
}
#endif
