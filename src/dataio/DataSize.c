#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <zlib.h>
#include <lzss.h>

#include "dataio/dat.h"
#include "dataio/BigEndian.h"

static char Dat1Check(char * path) {
	FILE * file = fopen(path,"rb");
	uint32_t Unknown[2];
	fseeko(file,4,SEEK_SET);
	fread(Unknown,4,2,file);
	fclose(file);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	Unknown[0] = FR_bswap32(Unknown[0]);
#endif
	if((Unknown[0] == 0x0A || Unknown[0] == 0x5E) && !Unknown[1]) return FO1_DAT;
	else return 0;
}

static char Dat2Check(char * path) {
	FILE * file = fopen(path,"rb");
	uint64_t arcsize;
	uint64_t t64;
	uint32_t t32;
	fseeko(file,-4,SEEK_END);
	fread(&t32,4,1,file);
	arcsize = ftello(file);
	fclose(file);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	t32 = FR_bswap32(t32);
#endif
	t64 = t32;
	if(t64 == arcsize) return FO2_DAT;
	else return 0;
}

static char DatXCheck(char * path) {
	fputs("DatX has not been drafted yet.\n",stderr);
	fprintf(stderr,"File path is %s\n",path);
	return 0;/*
	else return FR_DAT;
*/}

#ifndef BUILTIN_POPCNT
char FR_popcnt(char n) {
	n -= (n>>1) & 85;
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
	printf("Detected version: %i\n",(int)version);
	if(FR_popcnt(version & MULTIVERSION) > 1) return UNKNOWN_ERROR;
	else return version;
}

void FR_OpenDAT(char * path, struct fr_dat_handler_t * dat) {
	dat->control = IdentifyDat(path);
	if(dat->control & UNKNOWN_ERROR || !(dat->control & MULTIVERSION)) {
		dat->fp = NULL;
		fprintf(stderr,"%s is not a supported archive\n",path);
	} else {
		dat->control |= FILE_IS_OPEN;
		dat->fp = fopen(path,"rb");
	}
}

static void AllocateFile1(struct fo1_file_t * File, FILE * fp) {
	int16_t i = fgetc(fp) & 0x00ff;
	File->NameLength = i;
	File->Name = (char*)malloc(++i);
	fread(File->Name,File->NameLength,1,fp);
	File->Name[File->NameLength] = 0;
	fseek(fp,3,SEEK_CUR); /* Skipping big endian zeros */
	File->Attributes = fgetc(fp) >> 6;
	fread(&File->Offset,4,1,fp);
	fread(&(File->OrigSize),4,1,fp);
	fread(&(File->PackedSize),4,1,fp);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	File->Offset = FR_bswap32(File->Offset);
	File->OrigSize = FR_bswap32(File->OrigSize);
	File->PackedSize = FR_bswap32(File->PackedSize);
#endif
	File->Buffer = NULL;
}

static void NameDir1(struct fo1_dir_t * dir, FILE * fp) {
	int16_t l = fgetc(fp) & 0x00ff;
	dir->Length = l;
	dir->DirName = (char*)malloc(++l);
	fread(dir->DirName,dir->Length,1,fp);
	dir->DirName[dir->Length] = 0;
}

static void AssignFiles2Dir1(struct fo1_dir_t * dir, FILE * fp) {
	uint32_t i;
	fread(&dir->FileCount,4,1,fp);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	dir->FileCount = FR_bswap32(dir->FileCount);
#endif
	fseek(fp,12,SEEK_CUR);
	dir->File = (struct fo1_file_t*)malloc(dir->FileCount*sizeof(struct fo1_file_t));
	for(i = 0; i < dir->FileCount; ++i) AllocateFile1(&dir->File[i],fp);
}

static void AllocateDat1(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo1_dat_t * fo1 = dat->proxy = (struct fo1_dat_t*)malloc(sizeof(struct fo1_dat_t));
	fread(&fo1->DirectoryCount,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	fo1->DirectoryCount = FR_bswap32(fo1->DirectoryCount);
#endif
/* Skipping unknowns and magic */
	fseek(dat->fp,12,SEEK_CUR);
	fo1->Directory = (struct fo1_dir_t*)malloc(fo1->DirectoryCount*sizeof(struct fo1_dir_t));
	for(i = 0; i < fo1->DirectoryCount; ++i) NameDir1(&fo1->Directory[i], dat->fp);
	for(i = 0; i < fo1->DirectoryCount; ++i) AssignFiles2Dir1(&fo1->Directory[i],dat->fp);
}

static void AllocateFile2(struct fo2_file_t * dat, FILE * fp) {
	fread(&dat->NameLength,4,1,fp);
	
/*	fread(&dat->)*/
}

static void AllocateDat2(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo2_dat_t * fo2 = dat->proxy = (struct fo2_dat_t*)malloc(sizeof(struct fo2_dat_t));
	fseek(dat->fp,-8,SEEK_END);
	fread(&fo2->TreeSize,4,1,dat->fp);
	fread(&fo2->DatSize,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	fo2->TreeSize = FR_bswap32(fo2->TreeSize);
	fo2->DatSize = FR_bswap32(fo2->DatSize);
#endif
	fseek(dat->fp,fo2->DatSize - fo2->TreeSize - 12,SEEK_SET);
	fread(&fo2->FilesTotal,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	fo2->FilesTotal = FR_bswap32(fo2->FilesTotal);
#endif
	fo2->File = (struct fo2_file_t*)malloc(fo2->FilesTotal*sizeof(struct fo2_file_t));
	for(i = 0; i < fo2->TreeSize; ++i) AllocateFile2(&fo2->File[i],dat->fp);
}

static void AllocateDatX(struct fr_dat_handler_t * dat) {
	size_t i = (size_t)dat;
	i = i+i;
}

void FR_ReadDAT(struct fr_dat_handler_t * dat) {
	switch(dat->control & MULTIVERSION) {
		case 1: AllocateDat1(dat); break;
		case 2: AllocateDat2(dat); break;
		case 4: AllocateDatX(dat); break;
	}
}

static void CloseFile1(struct fo1_file_t * dat) {
	free(dat->Name);
	if(dat->Buffer != NULL) free(dat->Buffer);
}

static void CloseDir1(struct fo1_dir_t * dat) {
	uint32_t i;
	free(dat->DirName);
	for(i = 0; i < dat->FileCount; ++i) CloseFile1(&dat->File[i]);
	free(dat->File);
}

static void CloseDat1(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo1_dat_t * fo1 = dat->proxy;
	for(i = 0; i < fo1->DirectoryCount; ++i) CloseDir1(&fo1->Directory[i]);
	free(fo1->Directory);
}
/*
static CloseDat2(struct fr_dat_handler_t * dat) {
	struct fo2_dat_t * fo2 = dat->proxy;
	
}*/

void FR_CloseDAT(struct fr_dat_handler_t * dat) {
	switch(dat->control & MULTIVERSION) {
		case 1: CloseDat1(dat); break;
/*		case 2: CloseDat2(dat); break;
		case 4: CloseDatX(dat); break;
*/	}
	fclose(dat->fp);
	free(dat->proxy);
}
