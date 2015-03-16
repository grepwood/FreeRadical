#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/param.h> /* for MAXPATHLEN */
#include "dataio/dat.h"
#include "dataio/BigEndian.h"

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
	char * c = malloc(strlen_a+strlen_b+2);
	memcpy(c,a,strlen_a);
	c[strlen_a] = OS_SLASH;
	memcpy(c+strlen_a+1,b,strlen_b);
	c[strlen_a+strlen_b+1] = 0;
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
	char * text_buf = malloc(N+F-1);
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
	free(text_buf);
	return dwOutputIndex;
}

int f1unpack(struct fo1_file_t * file, const char * path, FILE * fp) {
	int result = strlen(path);
	size_t e;
	FILE * fo = NULL;
	char * a = NULL;
	char * b = NULL;
	uint16_t blockDesc;
	uint32_t TotalSize = 0;
	char * fpath = compound_strings(path,result,file->Name,file->NameLength);
	result = fseek(fp,file->Offset,SEEK_SET);
	b = malloc(file->OrigSize);
	if(file->PackedSize && file->Attributes == LZSS) {
		while(TotalSize < file->PackedSize) {
			e = FR_fread_b16(&blockDesc,fp);
			if(blockDesc & 0x8000) {
				TotalSize += (blockDesc & 0x7fff);
				e = fread(b+TotalSize,blockDesc & 0x7fff,1,fp); if (!e) puts("Failed fread!");
			} else {
				a = malloc(blockDesc & 0x7fff);
				e = fread(a,blockDesc & 0x7fff,1,fp); if(!e) puts("Failed fread!");
				TotalSize += LZSSDecode(a,blockDesc & 0x7fff,b+TotalSize);
				free(a);
			}
		}
	} else {
		e = fread(b,file->OrigSize,1,fp);
		if(!e) puts("Failed fread!");
	}
	fo = fopen(fpath,"wb");
	result = fwrite(b,file->OrigSize,1,fo);
	fclose(fo);
	free(b);
	free(fpath);
	if(result == 1) return 0;
	else return -1;
}

void f1undat(struct fr_dat_handler_t * dat, const char * path) {
	char * a = NULL;
	const size_t pathlen = strlen(path);
	struct fo1_dat_t * fo1 = dat->proxy;
	uint32_t i, j;
	int e;	
	for(e = i = 0; i < fo1->DirectoryCount && !e; ++i, free(a)) {
		a = compound_strings(path,pathlen,fo1->Directory[i].DirName,fo1->Directory[i].Length);
#ifndef WINDOWS
		unixify_path(a);
#endif
		e = mkpath(a,0755);
		printf("Files to extract: %u\n",fo1->Directory[i].FileCount);
		for(j = 0; j < fo1->Directory[i].FileCount && !e; ++j) {
/*			if(j == 0 || j == 17 || j == 38) {
*/				printf("Extracting file %i\n",j);
				e = f1unpack(&fo1->Directory[i].File[j],a,dat->fp);
				if(e) printf("e: %i\n",e);
/*			}
*/		}
	}
}

int main(int argc, char **argv) {
	struct fr_dat_handler_t dat;
	if(argc < 3) return 1;
	FR_OpenDAT(argv[1],&dat);
	FR_ReadDAT(&dat);
	switch(dat.control & MULTIVERSION) {
		case 0: puts("Not a Fallout archive"); break;
		case 1: f1undat(&dat,argv[2]); break;
		case 2: fputs("Not implemented yet.\n",stderr); break;
		case 4: fputs("Not implemented yet.\n",stderr); break;
	}
	FR_CloseDAT(&dat);
	return 0;
}
