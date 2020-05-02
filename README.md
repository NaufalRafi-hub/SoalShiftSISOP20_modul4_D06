# SoalShiftSISOP20_modul4_D06


1. [Soal 1](#1-soal-1)
2. [Soal 2](#2-soal-2)
3. [Soal 3](#3-soal-3)
4. [Soal 4](#4-soal-4)


### 1. Soal 1
### 2. Soal 2
### 3. Soal 3
### 4. Soal 4

Soal menginginkan untuk membuat log bernama fs.log di dalam directory home. log tersebut digunakan untuk menyimpan fungsi-fungsi yang terpanggil di dalam sisop linux.

pembagian fungsi ada dua yaitu warning dan info, dimana info dan warning dibedakan dengan fungsi syscall, rmdir, dan unlink untuk warning dan sisanya untuk info.


//untuk membedakan info dan warning 
```sh
//fungsi info
void finfo (char *text, char* path){
    char* info = "INFO";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char log[1000];
    sprintf(log, "[%s]::[%02d][%02d][%02d]-[%02d]:[%02d]:[%02d]::[%s]::[%s]", info, tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/rafi/fs.log", "a");  //directory penulisan
    fprintf(out, "%s\n", log);  
    fclose(out);  
    return 0;
}
//fungsi warning 
void fwarning (char *text, char* path)
{
    char* info = "WARNING";
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char log[1000];
//template penulisan
    sprintf(log, "[%s]::[%02d][%02d][%02d]-[%02d]:[%02d]:[%02d]::[%s]::[%s]", info, tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, text, path);
	FILE *out = fopen("/home/rafi/fs.log", "a");  //directory penulisan
    fprintf(out, "%s\n", log);  
    fclose(out);  
    return 0;
}
```
diatas merupakan fungsi untuk info dan warning, didalam fungsi terdapat variable info untuk level. variabel struct tm yang berisi localtime digunakan untuk mengambil waktu real untuk tahun, bulan, hari, jam, menit, detik. variabel text dan desc akan dipassing dari masing-masing fungsi yang terpanggil

```sh
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    finfo ("LS", path);
	int res;
	res = lstat(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_chmod(const char *path, mode_t mode)
{
    finfo("SETPERMISSION",path);
	int res;

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
    finfo("SETOWNER", path);
	int res;

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
    finfo ("CD", path);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
    finfo ("CAT", path);
	int fd;
	int res;

	(void) fi;
	fd = open(path, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    finfo ("MKDIR", path);
	int res;

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    finfo ("CREATE", path);
	int res;

	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
    fwarning ("REMOVE", path);
	int res;

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
    finfo ("LN", from);
	int res;

	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
    fwarning ("RMDIR", path);
	int res;

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    finfo ("MOVE", from);
	int res;

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    finfo ("TRUNCATE", path);
	int res;

	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    finfo("SETACCESS",path);
	int res;

	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    finfo ("OPEN", path);
	int res;

	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}


#ifdef HAVE_POSIX_FALLOCATE
static int xmp_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
    finfo("FALLOCATE",path);
	int fd;
	int res;

	(void) fi;

	if (mode)
		return -EOPNOTSUPP;

	fd = open(path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	close(fd);
	return res;
}
#endif

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
    finfo ("WRITE", path);
	int fd;
	int res;

	(void) fi;
	fd = open(path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}
```
diatas merupakan fungsi yang mewakilkan tiap fungsi-fungsi yang terpanggil dalam sistem operasi linux, fungsi diatas dibedakan untuk info dan warning dari masing-masing fungsi yang terpanggil di dalam fungsi. 

```sh
//berguna sebagai pointer yang langsung nunjuk ke fungsi
static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
        .chmod =xmp_chmod,
        .chown = xmp_chown,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
        .symlink = xmp_symlink,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
	.open = xmp_open,
	.read = xmp_read,
	.write = xmp_write,
};
```
struct disini digunakan sebagai pointer yang langsung menunjuk ke fungsi yang nantinya akan terpanggil

