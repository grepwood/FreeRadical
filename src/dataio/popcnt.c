#include <stdio.h>

char FR_popcnt(char n) {
	n -= (n>>1) & 85;
	n = (n & 51) + ((n >> 2) & 51);
	n = (n + (n >> 4)) & 15;
	return n & 0x7F;
}
