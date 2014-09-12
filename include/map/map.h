#define ITEMS		0
#define CRITTERS	1
#define SCENERY		2
#define WALLS		3
#define TILES		4
#define MISC		5
#define INTERFACE	6
#define INVENTORY	7
#define HEADS		8
#define BACKGROUND	9
#define MAX_POSITION 40000

/* Don't ask me why we need uint32_t for elevation,
 * which takes values from 0 to 2. Ask Interplay.
 * By the way, maps are big-endian. */

struct map_header
{
	char version; /* Originally uint32_t */
	char * filename; /* 17 if version is 20 or smaller, otherwise null-terminated inside of the file */
	int32_t default_player_position; /* Modulo 40000. Should be changed in the future when mapv21 is invented */
	char default_elevation; /* Originally int32_t */
	char player_orientation; /* Originally int32_t */
	int32_t NUM-LOCAL-VARS; /* Number of local variables for the map */
	int32_t SID; /* Script ID */
	int32_t elevation_flags;
/*	int32_t Unknown_1 = 1; */
	int32_t NUM-GLOBAL-VARS;
	int32_t map_id;
	uint32_t time_since_epoch; /* Yes the time is 32bit. */
/*	int32_t Unknown_array[44]; we just skip it */
}

struct map_variable
{
	int32_t * global;
	int32_t * local;
}

struct tile_id /* malloc to 80000 if map version is lower than 21. */
{
	uint16_t roof;
	uint16_t floor;
}
