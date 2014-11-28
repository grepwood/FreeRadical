#include <stdio.h>
#include <stdint.h>
#include "dataio/text.h"
#include "map/map.h"

uint32_t GetMapVersion(FILE * Map) {
	uint32_t result = 0;
	fread(&result,4,1,Map);
	return result;
}

/* Function for handling modern MAP files */
char * FR_textline(FILE * stream) {
	char * result = NULL;
	size_t len = 0;
	size_t offset = ftello(stream);
	char buf;
	do {
		buf = fgetc(stream);
		if(buf) ++len;
	} while(buf);
	fseeko(stream,offset,SEEK_SET);
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
		return result;
	}
	else return FR_textline(Map);
}

static void FillMapHeader(struct map_header * result, FILE * Map) {
	uint32_t buf;
	fread(buf,4,1,Map);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	buf = FR_bswap32(buf);
#endif
	result->filename = GetFileName(Map,result.version);
	fread(result->default_player_position,4,1,Map);
	fread(result->default_elevation,4,1,Map);
	fread(result->player_orientation,4,1,Map);
	fread(result->NUM_LOCAL_VARS,4,1,Map);
	fread(result->SID,4,1,Map);
	fread(result->elevation_flags,4,1,Map);
	fread(result->NUM-GLOBAL-VARS,4,1,Map);
	fread(result->map_id,4,1,Map);
	fread(result->time_since_epoch,4,1,Map);
}

tile_t tile = malloc(80000);
for(
