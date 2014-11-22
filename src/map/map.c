#include <stdio.h>
#include <stdint.h>
#include "map/map.h"

uint32_t GetMapVersion(FILE * Map) {
	uint32_t result = 0;
	fread(&result,4,1,Map);
	return result;
}

/* Function for handling modern MAP files */
char * textline(FILE * stream) {
	char * result = NULL;
	size_t len = 0;
	size_t offset = ftell(stream);
	char buf;
	do {
		buf = fgetc(stream);
		if(buf) ++len;
	} while(buf);
	fseek(stream,offset,SEEK_SET);
	result = malloc(len+1);
	if(result) {
		fread(result,len,1,stream);
		result[len]=0;
	}
	return result;
}

char * GetFileName(FILE * Map, unsigned char version) {
	char * result;
	if(version < 21) {
/* Fallout 1 and 2 feature MAP version 20 or lower, which
 * have a fixed length of 16 bytes, including dot and
 * extension. */
		result = malloc(17);
		fread(result,16,1,Map);
		result[16] = 0;
	}
	else {
		result = textline(Map);
	}
	return result;
}

struct map_header * FillMapHeader(FILE * Map) {
	struct map_header result;
	char endian = BigEndian();
	fread(buf,4,1,Map);
	if(!endian) {
		buf = FR_bswap32(buf);
	}
	result.filename = GetFileName(Map,result.version);
	fread(result.default_player_position,4,1,Map);
	fread(result.default_elevation,4,1,Map);
	fread(result.player_orientation,4,1,Map);
	fread(result.NUM-LOCAL-VARS,4,1,Map);
	fread(result.SID,4,1,Map);
	fread(result.elevation_flags,4,1,Map);
	fread(result.NUM-GLOBAL-VARS,4,1,Map);
	fread(result.map_id,4,1,Map);
	fread(result.time_since_epoch,4,1,Map);
	return &result;
}

tile_t tile = malloc(80000);
for(
