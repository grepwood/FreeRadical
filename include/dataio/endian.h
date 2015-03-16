#ifdef __APPLE__
#	include <libkern/OSByteOrder.h>
#	define FR_bswap16 OSSwapConstInt16
#	define FR_bswap32 OSSwapConstInt32
#	define FR_bswap64 OSSwapConstInt64
#elif defined __GNUC__ && defined linux
#	include <byteswap.h>
#	define FR_bswap64 __bswap_64
#	define FR_bswap32 __bswap_32
#	define FR_bswap16 __bswap_16
#else
#	error "Add support for your compiler"
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define FR_fread_l16(dest, file)	fread(dest,2,1,file);
#define FR_fread_l32(dest, file)	fread(dest,4,1,file);
#define FR_fread_l64(dest, file)	fread(dest,8,1,file);
size_t FR_fread_b16(int16_t * dest, FILE * file);
size_t FR_fread_b32(int32_t * dest, FILE * file);
size_t FR_fread_b64(int64_t * dest, FILE * file);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define FR_fread_b16(dest, file)	fread(dest,2,1,file);
#define FR_fread_b32(dest, file)	fread(dest,4,1,file);
#define FR_fread_b64(dest, file)	fread(dest,8,1,file);
size_t FR_fread_l16(int16_t * dest, FILE * file);
size_t FR_fread_l32(int32_t * dest, FILE * file);
size_t FR_fread_l64(int64_t * dest, FILE * file);
#endif
char BigEndian(void);
