#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <lzss.h>

#include "dataio/dat.h"
#include "dataio/text.h"
#include "dataio/BigEndian.h"

unsigned char DATX_MAGIC[8] = {0x66,0x72,0x67,0x77,0x54,0x78,0x42,0xF0};

static char Dat1Check(char * path) {
	FILE * file = fopen(path,"rb");
	uint32_t Unknown[2];
	uint64_t i;
	int e;
	fseeko(file,4,SEEK_SET);
	i = ftello(file);
	e = fread(Unknown,4,2,file);
	if(e != 2) fprintf(stderr,"Dat1Check couldn't read 2 dwords from 0x%lx\n",i);
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
	uint64_t t64 = ftello(file);
	uint32_t t32;
	int e;
	fseeko(file,-4,SEEK_END);
	e = fread(&t32,4,1,file);
	if(!e) fprintf(stderr,"Dat2Check failed to read dword at 0x%lx\n",t64);
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
	char a[8];
	FILE * fp = fopen(path,"rb");
	int e = fread(a,8,1,fp);
	if(!e) fputs("DatXCheck failed to read qword at 0x0\n",stderr);
	fclose(fp);
	if(!memcmp(a,DATX_MAGIC,8)) return FR_DAT;
	else return 0;
}

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
	int e;
	uint32_t o = ftello(fp);
	File->NameLength = i;
	File->Name = (char*)malloc(++i);
	e = fread(File->Name,File->NameLength,1,fp);
	if(e != 1) fprintf(stderr,"AllocateFile1 failed to read string at 0x%x\n",o);
	File->Name[File->NameLength] = 0;
	fseeko(fp,3,SEEK_CUR); /* Skipping big endian zeros */
	File->Attributes = fgetc(fp) >> 6;
	o = ftello(fp);
	e = fread(&File->Offset,4,1,fp);
	if(e != 1) fprintf(stderr,"AllocateFile1 failed to read dword at 0x%x\n",o);
	o = ftello(fp);
	e = fread(&(File->OrigSize),4,1,fp);
	if(e != 1) fprintf(stderr,"AllocateFile1 failed to read dword at 0x%x\n",o);
	o = ftello(fp);
	e = fread(&(File->PackedSize),4,1,fp);
	if(e != 1) fprintf(stderr,"AllocateFile1 failed to read dword at 0x%x\n",o);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	File->Offset = FR_bswap32(File->Offset);
	File->OrigSize = FR_bswap32(File->OrigSize);
	File->PackedSize = FR_bswap32(File->PackedSize);
#endif
	File->Buffer = NULL;
}

static void NameDir1(struct fo1_dir_t * dir, FILE * fp) {
	int16_t l = fgetc(fp) & 0x00ff;
	int e;
	uint32_t i = ftello(fp);
	dir->Length = l;
	dir->DirName = (char*)malloc(++l);
	e = fread(dir->DirName,dir->Length,1,fp);
	if(e != 1) fprintf(stderr,"NameDir1 failed to read string at 0x%x\n",i);
	dir->DirName[dir->Length] = 0;
}

static void AssignFiles2Dir1(struct fo1_dir_t * dir, FILE * fp) {
	uint32_t i = ftello(fp);
	int e = fread(&dir->FileCount,4,1,fp);
	if(!e) fprintf(stderr,"AssignFiles2Dir1 failed to read dword at 0x%x\n",i);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	dir->FileCount = FR_bswap32(dir->FileCount);
#endif
	fseeko(fp,12,SEEK_CUR);
	dir->File = (struct fo1_file_t*)malloc(dir->FileCount*sizeof(struct fo1_file_t));
	for(i = 0; i < dir->FileCount; ++i) AllocateFile1(&dir->File[i],fp);
}

static void AllocateDat1(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo1_dat_t * fo1 = dat->proxy = (struct fo1_dat_t*)malloc(sizeof(struct fo1_dat_t));
	i = fread(&fo1->DirectoryCount,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	fo1->DirectoryCount = FR_bswap32(fo1->DirectoryCount);
#endif
/* Skipping unknowns and magic */
	fseeko(dat->fp,12,SEEK_CUR);
	fo1->Directory = (struct fo1_dir_t*)malloc(fo1->DirectoryCount*sizeof(struct fo1_dir_t));
	for(i = 0; i < fo1->DirectoryCount; ++i) NameDir1(&fo1->Directory[i], dat->fp);
	for(i = 0; i < fo1->DirectoryCount; ++i) AssignFiles2Dir1(&fo1->Directory[i],dat->fp);
}

static void AllocateFile2(struct fo2_file_t * dat, FILE * fp) {
	uint32_t i = ftello(fp);
	int e = fread(&dat->NameLength,4,1,fp);
	if(!e) fprintf(stderr,"AllocateFile2 failed to read dword from 0x%x\n",i);
	if(dat->NameLength == 0xFFFFFFFF) {
		fputs("Fallout 2 filename exceeds 32bit bounds!\n",stderr);
		fseeko(fp,0xFFFFFFFF,SEEK_CUR);
	} else {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		dat->NameLength = FR_bswap32(dat->NameLength);
#endif
		dat->Name = (char*)malloc(dat->NameLength+1);
		e = fread(dat->Name,dat->NameLength,1,fp);
		dat->Name[dat->NameLength] = 0;
	}
	dat->Attributes = fgetc(fp) << 1;
	e = fread(&dat->OrigSize,4,1,fp);
	e = fread(&dat->PackedSize,4,1,fp);
	e = fread(&dat->Offset,4,1,fp);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	dat->OrigSize = FR_bswap32(dat->OrigSize);
	dat->PackedSize = FR_bswap32(dat->PackedSize);
	dat->Offset = FR_bswap32(dat->Offset);
#endif
	dat->Buffer = NULL;
}

static void AllocateDat2(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo2_dat_t * fo2 = dat->proxy = (struct fo2_dat_t*)malloc(sizeof(struct fo2_dat_t));
	fseeko(dat->fp,-8,SEEK_END);
	i = fread(&fo2->TreeSize,4,1,dat->fp);
	i = fread(&fo2->DatSize,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	fo2->TreeSize = FR_bswap32(fo2->TreeSize);
	fo2->DatSize = FR_bswap32(fo2->DatSize);
#endif
	fseeko(dat->fp,fo2->DatSize - fo2->TreeSize - 8,SEEK_SET);
	i = fread(&fo2->FilesTotal,4,1,dat->fp);
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	fo2->FilesTotal = FR_bswap32(fo2->FilesTotal);
#endif
	printf("Files total: %u\n",fo2->FilesTotal);
	fo2->File = (struct fo2_file_t*)malloc(fo2->FilesTotal*sizeof(struct fo2_file_t));
	for(i = 0; i < fo2->FilesTotal; ++i) AllocateFile2(&fo2->File[i],dat->fp);
}

static void AllocateFileX(struct fr_file_t * dat, FILE * fp) {
	int e;
	uint64_t i = ftello(fp);
	dat->Name = FR_textline(fp);
	dat->Attributes = fgetc(fp);
	e = fread(&dat->Offset,8,1,fp);
	if(!e) fprintf(stderr,"AllocateFileX failed to read Offset at 0x%lx\n",i);
	i = ftello(fp);
	e = fread(&dat->OrigSize,8,1,fp);
	if(!e) fprintf(stderr,"AllocateFileX failed to read Offset at 0x%lx\n",i);
	i = ftello(fp);
	e = fread(&dat->PackedSize,8,1,fp);
	if(!e) fprintf(stderr,"AllocateFileX failed to read Offset at 0x%lx\n",i);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	dat->Offset = FR_bswap64(dat->Offset);
	dat->OrigSize = FR_bswap64(dat->OrigSize);
	dat->PackedSize = FR_bswap64(dat->PackedSize);
#endif
	dat->Buffer = NULL;
}

static void AllocateDatX(struct fr_dat_handler_t * dat) {
	uint64_t i = ftello(dat->fp);
	int e;
	struct fr_dat_t * fr = dat->proxy = (struct fr_dat_t*)malloc(sizeof(struct fr_dat_t));
	fseeko(dat->fp,8,SEEK_SET);
	e = fread(&fr->FileCount,8,1,dat->fp);
	if(!e) fprintf(stderr,"AllocateDatX failed to read file count at 0x%lx\n",i);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	fr->FileCount = FR_bswap64(fr->FileCount);
#endif
	fr->File = (struct fr_file_t*)malloc(fr->FileCount*sizeof(struct fr_file_t));
	for(i = 0; i < fr->FileCount; ++i) AllocateFileX(&fr->File[i],dat->fp);
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

static void CloseFile2(struct fo2_file_t * dat) {
	free(dat->Name);
	if(dat->Buffer != NULL) free(dat->Buffer);
}

static void CloseDat2(struct fr_dat_handler_t * dat) {
	uint32_t i;
	struct fo2_dat_t * fo2 = dat->proxy;
	for(i = 0; i < fo2->FilesTotal; ++i) CloseFile2(&fo2->File[i]);
	free(fo2->File);
}

static void CloseFileX(struct fr_file_t * dat) {
	free(dat->Name);
	if(dat->Buffer != NULL) free(dat->Buffer);
}

static void CloseDatX(struct fr_dat_handler_t * dat) {
	uint64_t i;
	struct fr_dat_t * fr = dat->proxy;
	for(i = 0; i < fr->FileCount; ++i) CloseFileX(&fr->File[i]);
	free(fr->File);
}

void FR_CloseDAT(struct fr_dat_handler_t * dat) {
	switch(dat->control & MULTIVERSION) {
		case 1: CloseDat1(dat); break;
		case 2: CloseDat2(dat); break;
		case 4: CloseDatX(dat); break;
	}
	fclose(dat->fp);
	dat->control &= ~FILE_IS_OPEN;
	free(dat->proxy);
}
