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

#define ELEVATION_UNKNOWN 1
#define ELEVATION_L0 2
#define ELEVATION_L1 4
#define ELEVATION_L2 8

#define MAP_OBJ_HELD_RH 1
#define MAP_OBJ_HELD_LH 2
#define MAP_OBJ_WORN 4

/* Don't ask me why we need uint32_t for elevation,
 * which takes values from 0 to 2. Ask Interplay.
 * By the way, maps are big-endian. */

#define F1_EPOCH "05 Dec 2161 00:00"
#define F2_EPOCH "25 Jul 2241 00:00"

struct map_header {
	uint8_t version; /* Originally uint32_t */
	char * filename; /* 17 if version is 20 or smaller, otherwise null-terminated inside of the file */
	int32_t default_player_position; /* Modulo 40000. Should be changed in the future when mapv21 is invented */
	uint8_t default_elevation; /* Originally int32_t */
	uint8_t player_orientation; /* Originally int32_t */
	int32_t NUM-LOCAL-VARS; /* Number of local variables for the map */
	int32_t SID; /* Script ID */
	uint8_t elevation_flags;
	int32_t NUM-GLOBAL-VARS;
	int32_t map_id;
	uint32_t time_since_epoch; /* Yes the time is 32bit. */
}

struct map_variable {
	int32_t * global;
	int32_t * local;
};

/* Tiles */

/* malloc to 80000 if map version is lower than 21. */
struct tile_t {
	uint16_t roof;
	uint16_t floor;
};

/* Map scripts */

struct map_script_t {
	uint8_t PID; /* 5 possible */
	int32_t unknown1;
	int32_t unknown2;
	int32_t unknown3;
	int32_t ScriptID;
	int32_t unknown4;
	int32_t unknown5;
	int32_t unknown7;
	int32_t unknown8;
	int32_t unknown9;
	int32_t unknownA;
	int32_t unknownC;
	int32_t unknownD;
	int32_t unknownE;
	int32_t unknownF;
/* Omitted unknown 0, 6 and B because they're fixed to -1 */
};

/* Map objects */
struct map_object_t {
	uint32_t parent_counter;
	int32_t position; /* -1 to 39999. -1 means it's not on the map, like in inventory */
	uint32_t frameno; /* from FRM file */
	uint8_t orientation; /* 0-5 */
	uint32_t PROTO-PID; /* from FRM file */
	uint8_t flags;
	uint8_t elevation; /* 0-2 */
	uint32_t light;
	uint32_t radiation;
	uint32_t PID;
	int32_t ScriptID; /* -1 means no script */
	uint32_t inventory;
};

/* Extra critter fields */
struct ECF_t {
	int32_t AI_packet_no;
	uint32_t GID;
	uint32_t HP;
};

/* Extra key fields */
struct EKF_t {
	uint32_t door;
};

/* Extra weapon fields */
struct EWF_t {
	uint32_t ammo;
	int32_t ammo_PROTO;
};

/* Extra fields for ladder bottom */
struct EFLB_t {
	uint32_t unknown2;
};

/* Extra fields for ladder top */
struct EFLT_t {
	uint32_t unknown2;
};

/* Extra fields for portals */
struct EFP_t {
	uint32_t unknown1;
};

struct EFS_t {
	uint32_t unknown1;
	uint32_t unknown2;
};

struct EFE_t {
	uint32_t current_floor;
	uint32_t floors;
};

struct EFEG_t {
	uint32_t DESTINATION-MAP-ID;
	uint16_t dest_player_position;
	uint8_t dest_elevation;
	uint8_t dest_orientation;
};
