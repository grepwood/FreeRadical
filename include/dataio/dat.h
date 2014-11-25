#define FO1_DAT 1
#define FO2_DAT 2
#define FOX_DAT 4

#define PLAINTEXT	0
#define LZSS		1
#define ZLIB		2

#include <stdio.h>

/*#ifdef LEGACY_SUPPORT */
/* Fallout 1 DAT structure */
struct fo1_file_t {
	unsigned char NameLength;	/* FO1 uses Pascal strings... */
	char * Name;
	char Attributes;			/* Originally 0x20 for plaintext and 0x40 for LZSS
								 * Implicitly shifted to the right by 6 by FR */
	uint32_t Offset;			/* Offset wrt the .dat */
	uint32_t OrigSize;
	uint32_t PackedSize;		/* 0 if not compressed */
	char * Buffer;				/* FreeRadical's way of telling if it's buffered into the memory or not */
};

struct fo1_dir_t {
	unsigned char Length;
	char * DirName;
	uint32_t FileCount;
	struct fo1_file_t * File;
};

struct fo1_dat_t {
	uint32_t DirectoryCount;
	struct fo1_dir_t * Directory;
};

/* Fallout 2 DAT structure */
struct fo2_file_t {
	uint32_t NameLength;	/* Really? */
	char * Name;
	char Attributes; 		/* Boolean, implicitly shifted to the right by 1
							 * so now we can deploy a universal switch statement against
							 * compression attribute of files from FO1 and FO2 */
	uint32_t OrigSize;
	uint32_t PackedSize;
	uint32_t Offset;
	char * Buffer;
};

struct fo2_dat_t {
/* Info block */
	uint32_t DatSize;
	uint32_t FilesTotal;
	uint32_t TreeSize;
/* Archive block */
	struct fo2_file_t * File;
};
/*#endif*/

/* This will be defined in the future
struct fr_dat_t {
};*/

struct fr_dat_handler_t {
	FILE * fp;
	char control;
	void * proxy;
};
