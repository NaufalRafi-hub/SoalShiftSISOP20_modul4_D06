#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char *dirpath = "/home/syamil/Documents";
char key[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
const int temp = 10;

char *getExt (char *str) {
    char *ext = strrchr (str, '.');
    if (ext == NULL)
        ext = "";
    return ext;
}

void encrypt1(char *enc){
	if(!strcmp(enc,".") || !strcmp(enc,"..")) return;
	int len=strlen(getExt(enc));
	for( int i = 0; i < strlen(enc)-len ;i++) {
		if(enc[i] != '/'){
			for (int j = 0; j < strlen(key); j++) {
	     		if(enc[i] == key[j]) {
	        		enc[i] = key[(j+temp) % strlen(key)];
	        		break;
        	}
			}
		}
	}
}


//Get file attributes.
static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
    sprintf(fpath,"%s/%s",dirpath,encv1);
	}

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


//Read directory
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
	char *encv1 = strstr(path,"encv1_");
	sprintf(fpath, "%s%s",dirpath,path);
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}else{  		
		if(encv1 != NULL ){
     		sprintf(fpath,"%s/%s",dirpath,encv1);
     }
  	}
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
  		st.st_mode = de->d_type << 12;

		if(encv1 != NULL){
			encrypt1(de->d_name);
		}
    	res = (filler(buf, de->d_name, &st, 0));
		if(res!=0) 
			break;
	}
	closedir(dp);
	return 0;
}

//Read data from an open file
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;
  char fpath[1000];
  char *encv1 = strstr(path,"encv1_");
	sprintf(fpath, "%s%s",dirpath,path);
	
	if(encv1 != NULL){
		sprintf(fpath,"%s/%s",dirpath,encv1);
	}
	
	(void) fi;
	
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;
	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	close(fd);
	return res;
}

//Create a directory.
static int xmp_mkdir(const char *path, mode_t mode)
{
        int res;
		char fullpath[1000];
		char *encv1 = strstr(path,"encv1_");
		sprintf(fullpath ,"%s%s",dirpath,path);
		
		if(encv1 != NULL){
			sprintf(fullpath,"%s/%s",dirpath,encv1);
		}
		
        res = mkdir(fullpath, mode);
        if (res == -1)
                return -errno;
        return 0;
}

//Rename a file
static int xmp_rename(const char *from, const char *to)
{
     int res;
		 res = rename(from, to);
     if (res == -1)
            return -errno;
     return 0;
}

static struct fuse_operations xmp_oper = 
{
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mkdir		= xmp_mkdir,
	.rename		= xmp_rename,
	.read			= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
