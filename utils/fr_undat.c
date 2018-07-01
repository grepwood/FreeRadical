#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <zlib.h>
#include <sys/param.h> /* for MAXPATHLEN */
#include "dataio/dat.h"
#include "dataio/endian.h"

/* Copyright 2015 Michael Dec <grepwood@sucs.org>
 * This has only been tested on Linux. Run on Windows at your own risk!
 */

#ifdef WINDOWS
#	define OS_SLASH '\\'
#else
void unixify_path(char * path) {
	size_t i, j;
	for(j = 0, i = strlen(path); j < i; ++j) if(path[j] == '\\') path[j] = '/';
}
#	define OS_SLASH '/'
#endif

int mkpath(const char *s, mode_t mode){
	char *q, *r = NULL, *path = NULL, *up = NULL;
	int rv = -1;
	if (strcmp(s, ".") == 0 || strcmp(s, "/") == 0) return (0);
	if ((path = strdup(s)) == NULL) exit(1);
	if ((q = strdup(s)) == NULL) exit(1);
	if ((r = dirname(q)) == NULL) goto out;
	if ((up = strdup(r)) == NULL) exit(1);
	if ((mkpath(up, mode) == -1) && (errno != EEXIST)) goto out;
	if ((mkdir(path, mode) == -1) && (errno != EEXIST)) rv = -1;
	else rv = 0;
out:
 	if (up != NULL) free(up);
	free(q);
	free(path);
	return (rv);
}

char * compound_strings(const char * a, unsigned char strlen_a, const char * b, unsigned char strlen_b) {
	char * c = NULL;
	if(a[strlen_a-1] != OS_SLASH) {
		c = malloc(strlen_a+strlen_b+2);
		memcpy(c,a,strlen_a);
		c[strlen_a] = OS_SLASH;
		memcpy(c+strlen_a+1,b,strlen_b);
		c[strlen_a+strlen_b+1] = 0;
	} else {
		c = malloc(strlen_a+strlen_b+1);
		memcpy(c,a,strlen_a);
		memcpy(c+strlen_a,b,strlen_b);
		c[strlen_a+strlen_b] = 0;
	}
	return c;
}

#define N 4096
#define F 18
#define THRESHOLD 2
#define GETBYTE() ((dwInputCurrent < dwInputLength) ? buffer[dwInputCurrent++] & 0x00FF : 0xffff)
int LZSSDecode(char* buffer, int dwInputLength, char* arOut) {
	uint16_t i, j, k, c;
	int dictionaryIndex = N - F;
	uint16_t flags = 0;
	int dwInputCurrent = 0;
	int dwOutputIndex = 0;
	char text_buf[N+F-1];
	memset(text_buf,' ',dictionaryIndex);
	for ( ; ; ) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = GETBYTE()) == 0xffff) break;
			flags = c | 0xff00;		/* uses higher byte to count 8 */
		}
		if (flags & 1) {
			if ((c = GETBYTE()) == 0xffff) break;
			arOut[dwOutputIndex++] = c;
			text_buf[dictionaryIndex++] = c;
			dictionaryIndex &= (N - 1);
		} else {
			if ((i = GETBYTE()) == 0xffff) break;
			if ((j = GETBYTE()) == 0xffff) break;
			i |= ((j & 0xf0) << 4);
			j = (j & 0x0f) + THRESHOLD;
			for (k = 0; k <= j; k++) {
				c = text_buf[(i + k) & (N - 1)];
				arOut[dwOutputIndex++] = c;
				text_buf[dictionaryIndex++] = c;
				dictionaryIndex &= (N - 1);
			}
		}
	}
	return dwOutputIndex;
}

int ungz(char * dst, const uint32_t dstLen, char * src, const uint32_t srcLen) {
	int err = -1;
	int ret = -1;
	z_stream strm  = {0};
	strm.total_in  = strm.avail_in  = srcLen;
	strm.total_out = strm.avail_out = dstLen;
	strm.next_in   = (unsigned char *)src;
	strm.next_out  = (unsigned char *)dst;
	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;
	err = inflateInit2(&strm, 47); /* 15 window bits, and the +32 tells zlib to to detect if using gzip or zlib */
	if (err == Z_OK) {
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END) ret = strm.total_out;
		else {
			inflateEnd(&strm);
			return err;
		}
	} else {
		inflateEnd(&strm);
		return err;
	}
	inflateEnd(&strm);
	return ret;
}

int f2unpack(struct fo2_file_t * file, const char * path, FILE * fp) {
	FILE * fo = NULL;
	char * a = NULL;
	char * b = NULL;
	int result;
	size_t e;
/* Debug section */
#ifdef DEBUG
	printf("File: %s\n",file->Name);
	printf("OrigSize: %i\n",file->OrigSize);
	printf("PackedSize: %i\n",file->PackedSize);
	printf("Offset: 0x%x\n",file->Offset);
#endif /* DEBUG */
	if(file->OrigSize) {
		result = fseeko(fp,file->Offset,SEEK_SET);
		b = malloc(file->OrigSize);
		if(file->PackedSize && file->Attributes == THE_ZLIB) {
			a = malloc(file->PackedSize);
			e = fread(a,file->PackedSize,1,fp); if (!e) puts("Failed fread!");
			ungz(b,file->OrigSize,a,file->PackedSize);
			free(a);
		} else {
			e = fread(b,file->OrigSize,1,fp);
			if (!result) puts("Failed fread!");
		}
	}
	fo = fopen(path,"wb");
	result = 0;
	if(fo == NULL) puts("Couldn't open file for writing!");
	else	if(file->OrigSize) result = fwrite(b,file->OrigSize,1,fo);
			else result = 1;
	fclose(fo);
	free(b);
	if(result == 1) return 0;
	else return -1;
}

int f1unpack(struct fo1_file_t * file, const char * path, FILE * fp) {
	size_t result = strlen(path);
	FILE * fo = NULL;
	char * a = NULL;
	char * b = NULL;
	uint16_t blockDesc;
	uint32_t TotalSize = 0;
	char * fpath = compound_strings(path,result,file->Name,file->NameLength);
/* Debug section */
#ifdef DEBUG
	printf("File: %s\n",file->Name);
	printf("OrigSize: %i\n",file->OrigSize);
	printf("PackedSize: %i\n",file->PackedSize);
	printf("Offset: 0x%x\n",file->Offset);
#endif /* DEBUG */
	if(file->OrigSize) {
		result = fseek(fp,file->Offset,SEEK_SET);
		b = malloc(file->OrigSize);
		if(file->PackedSize && file->Attributes == LZSS) {
			while(TotalSize < file->OrigSize) {
				result = FR_fread_b16(&blockDesc,fp);
				if(blockDesc & 0x8000) {
					blockDesc &= 0x7fff;
					result = fread(b+TotalSize,blockDesc,1,fp); if (!result) puts("Failed fread!");
					TotalSize += blockDesc;
				} else {
					a = malloc(blockDesc);
					result = fread(a,blockDesc,1,fp); if(!result) puts("Failed fread!");
					TotalSize += LZSSDecode(a,blockDesc,b+TotalSize);
					free(a);
				}
			}
		} else {
			result = fread(b,file->OrigSize,1,fp);
			if(!result) puts("Failed fread!");
		}
	}
	fo = fopen(fpath,"wb");
	if(fo == NULL) puts("Couldn't open file for writing!");
	else	if(file->OrigSize) result = fwrite(b,file->OrigSize,1,fo);
			else result = 1;
	fclose(fo);
	free(b);
	free(fpath);
	if(result == 1) return 0;
	else return -1;
}

void cutoff_filename(char * path, uint32_t len) {
	while(path[len] != OS_SLASH) --len;
	path[len] = 0;
}

void bring_back_filename(char * b) {
	uint32_t i = 0;
	while(b[i]) ++i;
	b[i] = OS_SLASH;
}

void f2undat(struct fr_dat_handler_t * dat, const char * path) {
	char * b = NULL;
	struct fo2_dat_t * fo2 = dat->proxy;
	uint32_t i;
	int e;
	for(i = 0; i < fo2->FilesTotal; ++i, free(b)) {
#ifndef WINDOWS
		unixify_path(fo2->File[i].Name);
#endif
		b = compound_strings(path,strlen(path),fo2->File[i].Name,fo2->File[i].NameLength);
		cutoff_filename(b,strlen(b));
		e = mkpath(b,0755);
		bring_back_filename(b);
		e = f2unpack(&fo2->File[i],b,dat->fp);
		if(e) printf("e: %i\n",e);
	}
}

void f1undat(struct fr_dat_handler_t * dat, const char * path) {
	char * a = NULL;
	const size_t pathlen = strlen(path);
	struct fo1_dat_t * fo1 = dat->proxy;
	uint32_t i, j;
	int e;
	for(e = i = 0; i < fo1->DirectoryCount && !e; ++i, free(a)) {
#ifndef WINDOWS
		unixify_path(fo1->Directory[i].DirName);
#endif
		a = compound_strings(path,pathlen,fo1->Directory[i].DirName,fo1->Directory[i].Length);
		e = mkpath(a,0755);
		for(j = 0; j < fo1->Directory[i].FileCount && !e; ++j) {
			e = f1unpack(&fo1->Directory[i].File[j],a,dat->fp);
			if(e) printf("e: %i\n",e);
		}
	}
}

int rtfm(char * exe) {
	printf("Usage: %s file.dat output-directory\n",exe);
	return 1;
}

int main(int argc, char **argv) {
	struct fr_dat_handler_t dat;
	if(argc < 3) return rtfm(argv[0]);
	FR_OpenDAT(argv[1],&dat);
	FR_ReadDAT(&dat);
	switch(dat.control & MULTIVERSION) {
		case 0: puts("Not a Fallout archive"); break;
		case 1: f1undat(&dat,argv[2]); break;
		case 2: f2undat(&dat,argv[2]); break;
		case 4: fputs("Not implemented yet.\n",stderr); break;
	}
	FR_CloseDAT(&dat);
	return 0;
}
