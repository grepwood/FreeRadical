#include <stdio.h>
/*
char FR_popcnt(char n) {
	n -= (n>>1) & 85;
	n = (n & 51) + ((n >> 2) & 51);
	n = (n + (n >> 4)) & 15;
	return n & 0x7F;
}
* Might not use this at all since we only check first 3 bits
*/
char FR_popcnt(const char n) {
	return ((n&4)>>2) + ((n&2)>>1) + (n&1);
}
