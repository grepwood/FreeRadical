#ifdef __GNUC__
#	include <byteswap.h>
	#define FR_bswap64 __bswap_64
	#define FR_bswap32 __bswap_32
	#define FR_bswap16 __bswap_16
#else
#	error "Add support for your compiler"
#endif
char BigEndian(void);
