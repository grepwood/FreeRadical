#include <stdint.h>
#include "dat.h"
#include <zlib.h>
#include <lzss.h>

#include "include/dataio/BigEndian.h"

static char Dat1Check(char * path) {
	FILE * file = fopen(path,"rb");
	int Unknown[2];
	fseek(file,3,SEEK_SET);
	fread(Unknown,4,2,file);
	fclose(file);
#ifndef BIG_ENDIAN
	Unknown[0] = FR_bswap32(Unknown[0]);
#endif
	if((Unknown[0] == 0x0A || Unknown[0] == 0x5E) && !Unknown[1]) return 1;
	else return 0;
}

static char Dat2Check(char * path) {
	FILE * file = fopen(path,"rb");
	register size_t FileSize;
	int SFF;
	fseek(file,0,SEEK_END);
	FileSize = ftell(file);
	fseek(file,-4,SEEK_END);
	fread(&SFF,4,1,file);
	fclose(file);
#ifdef BIG_ENDIAN
	SFF = FR_bswap32(SFF);
#endif
	if(SFF == FileSize) return 2;
	else return 0;
}

static char DatXCheck(char * path) {
	fputs("DatX has not been drafted yet.\n",stderr);
	return 0;/*
	else return 4;
*/}

#ifndef BUILTIN_POPCNT
char FR_popcnt(char n) {
/*	static const char m1 = 85; 255/3
	static const char m2 = 51; 255/5
	static const char m4 = 15; 255/17
*/	n -= (n>>1) & 85;
	n = (n & 51) + ((n >> 2) & 51);
	n = (n + (n >> 4)) & 15;
	return n & 0x7F;
}
#else
#	ifdef GNUC
#		define FR_popcnt __builtin_popcount
#	endif
#endif

static char IdentifyDat(char * path) {
	char version = Dat1Check(path) | Dat2Check(path) | DatXCheck(path);
	if(FR_popcnt(version) > 1) return 0xF0;
	else return version;
}

void FR_OpenDAT(char * path, struct fr_dat_handler_t * dat) {
	dat->flag = IdentifyDat(path);
	if(dat->flag.error || !dat->flag.version) {
		dat->fp = NULL;
		fprintf(stderr,"%s is not a supported archive\n",path);
	} else {
		dat->flag.open = 1;
		dat->fp = fopen(path,"rb");
	}
}

static char * ReadDirName1(FILE * fp) {
	unsigned char i = fgetc(fp);
	char * result = malloc(i+1);
	static long err;
	if(result == NULL) return NULL;
	err = fread(result,i+1,1,fp);
	if(err == 1) return result;
	else {
		free(result);
		fprintf(stderr,"ReadDirName1 has fread %lu instead 1\n",err);
		return NULL;
	}
}

static void AllocateFile1(struct fo1_file_t * File, FILE * fp) {
	int16_t i = fgetc(fp) & 0x00ff;
	File->NameLength = i;
	File->Name = malloc(++i);
	fread(File->Name,File->NameLength,1,fp);
	File->Name[File->NameLength] = 0;
	fseek(fp,3,SEEK_SET); /* Skipping big endian zeros */
	File->Attributes = (fgetc(fp) & 0x000000ff) >> 6;
	fread(&File->Offset,4,1,fp);
	fread(&File->OrigSize,4,1,fp);
	fread(&File->PackedSize,4,1,fp);
#ifndef BIG_ENDIAN
	File->Offset = FR_bswap32(File->Offset);
	File->OrigSize = FR_bswap32(File->OrigSize);
	File->PackedSize = FR_bswap32(File->PackedSize);
#endif
}

static void AllocateDir1(struct fo1_dir_t * dir, FILE * fp) {
	int16_t l = fgetc(fp) & 0x00ff;
	long i;
	dir->Length = l;
	dir->DirName = malloc(++l);
	fread(dir->DirName,dir->Length,1,fp);
	dir->DirName[dir->Length] = 0;
	fread(&dir->FileCount,4,1,fp);
#ifndef BIG_ENDIAN
	dir->FileCount = FR_bswap32(dir->FileCount);
#endif
	fseek(fp,12,SEEK_CUR);
	dir->File = malloc(dir->FileCount*sizeof(fo1_file_t));
	for(i = 0; i < dir->FileCount; ++i) AllocateFile1(dir->File[i],fp);
}

static void AllocateDat1(struct fr_dat_handler_t * dat) {
	uint32_t CurrentDir;
	struct fo1_dat_t * fo1 = dat->proxy = malloc(sizeof(fo1_dat_t));
	fread(&fo1->DirectoryCount,4,1,dat->fp);
#ifndef BIG_ENDIAN
	fo1->DirectoryCount = FR_bswap32(fo1->DirectoryCount);
#endif
	fseek(dat->fp,12,SEEK_CUR); /* Skipping unknowns and magic */
	fo1->Directory = malloc(fo1->DirectoryCount*sizeof(fo1_dir_t));
	for(CurrentDir = 0; CurrentDir < fo1->DirectoryCount; ++CurrentDir) {
		AllocateDir1(fo1->Directory[CurrentDir], datfile->fp);
	}
}

static void AllocateFile2(struct fo2_file_t * dat, FILE * fp) {
	fread(&dat->NameLength,4,1,fp);
	
	fread(&dat->
}

static void AllocateDat2(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo2_dat_t * fo2 = dat->proxy = malloc(sizeof(fo2_dat_t));
	fseek(dat->fp,-8,SEEK_END);
	fread(&dat->TreeSize,4,1,dat->fp);
	fread(&dat->DatSize,4,1,dat->fp);
#ifdef BIG_ENDIAN
	dat->TreeSize = FR_bswap32(dat->TreeSize);
	dat->DatSize = FR_bswap32(dat->DatSize);
#endif
	fseek(dat->fp,dat->DatSize - dat->TreeSize - 12,SEEK_SET);
	fread(&dat->FilesTotal,4,1,dat->fp);
#ifdef BIG_ENDIAN
	dat->FilesTotal = FR_bswap32(dat->FilesTotal);
#endif
	dat->File = malloc(dat->FilesTotal*sizeof(fo2_file_t));
	for(i = 0; i < dat->TreeSize; ++i) AllocateFile2(dat->File[i],dat->fp);
}

static void AllocateDatX(struct fr_dat_handler_t * dat) {
	int i = 0;
	i = i+i;
}

void FR_ReadDAT(struct fr_dat_handler_t * dat) {
	switch(dat->flag.version) {
		case 1: AllocateDat1(dat); break;
		case 2: AllocateDat2(dat); break;
		case 4: AllocateDatX(dat); break;
	}
}
