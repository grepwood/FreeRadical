#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lzss.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/param.h> /* for MAXPATHLEN */
#include "dataio/dat.h"
#ifdef WINDOWS
#	define OS_SLASH '\\'
#else
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

int f1unpack(struct fo1_file_t * file, const char * path, FILE * fp) {
	int result = strlen(path);
	FILE * fo = NULL;
	struct lzss_t a = {NULL,0,0};
	struct lzss_t b = {NULL,0,0};
	char * fpath = compound_strings(path,result,file->Name,file->NameLength);
	printf("UnDATing %s:\n",fpath);
	printf("\tJumping to offset 0x%x\n",file->Offset);
	result = fseek(fp,SEEK_SET,file->Offset);
	if(!result) printf("\tSuccessfully jumped to offset 0x%x\n",file->Offset);
	else printf("\tfseek returned 0x%x\n",result);
	printf("\tOriginal size: %i\n",file->OrigSize);
	if(file->PackedSize && file->Attributes == LZSS) {
		puts("\tCompression: LZSS");
		printf("\tPacked size: %i\n",file->PackedSize);
		a.ptr = malloc(file->PackedSize);
		a.size = file->PackedSize;
		fread(a.ptr,file->PackedSize,1,fp);
		lzss_decode_mm(&a,&b);
		printf("\tliblzss predicted output %i\n",b.size);
		free(a.ptr);
	}
	else {
		b.ptr = malloc(file->OrigSize);
		puts("\tCompression: none");
		fread(b.ptr,file->OrigSize,1,fp);
	}
	if(result) {
		fo = fopen(fpath,"wb");
/*		result = fwrite(b.ptr,file->OrigSize,1,fo);
*/		fclose(fo);
	}
	free(b.ptr);
	free(fpath);
	if(result == 1) return 0;
	else return -1;
}

void unixify_path(char * path) {
	size_t i, j;
	for(j = 0, i = sizeof(path); j < i; ++j) if(path[j] == '\\') path[j] = '/';
}


void f1undat(struct fr_dat_handler_t * dat, const char * path) {
	char * a = NULL;
	const size_t pathlen = strlen(path);
	struct fo1_dat_t * fo1 = dat->proxy;
	size_t substring;
	uint32_t i, j;
	int e;	
	for(e = i = 0; i < fo1->DirectoryCount && !e; ++i, free(a)) {
		substring = pathlen + fo1->Directory[i].Length;
		a = malloc(substring+2);
		memcpy(a,path,pathlen);
#ifndef WINDOWS
		unixify_path(fo1->Directory[i].DirName);
#endif
		a[pathlen] = OS_SLASH;
		memcpy(a+pathlen+1,fo1->Directory[i].DirName,fo1->Directory[i].Length);
		a[substring+1] = 0;
		e = mkpath(a,0755);
/*		for(j = 0; j < fo1->Directory[i].FileCount && !e; ++j) {
*/		for(j = 0; j < 1 && !e; ++j) {
			e = f1unpack(&fo1->Directory[i].File[j],a,dat->fp);
			printf("e: %i\n",e);
		}
	}
}

int main(int argc, char **argv) {
	struct fr_dat_handler_t dat;
	if(argc < 3) return 1;
	FR_OpenDAT(argv[1],&dat);
	FR_ReadDAT(&dat);
	switch(dat.control & MULTIVERSION) {
		case 1: f1undat(&dat,argv[2]); break;
		case 2: fputs("Not implemented yet.\n",stderr); break;
		case 4: fputs("Not implemented yet.\n",stderr); break;
	}
	FR_CloseDAT(&dat);
	return 0;
}
