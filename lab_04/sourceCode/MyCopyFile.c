# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/time.h>
# include <sys/types.h>
# include <dirent.h>
# include <unistd.h>
# include <fcntl.h>
# include <utime.h>


void MyCopy(const char *src, const char *dest);
void copyLinkFile(const char *src, const char *dest);
void copyFile(const char *src, const char *dest);

int main(int argc, char** argv){
    struct stat statbuf;		//stat用于存储文件各项属性信息
    struct utimbuf utb;			//
    DIR *dirSrc = NULL;			//指向打开的目录
    DIR *dirDest = NULL;
    ///如果命令行参数超过三个
	if (argc != 3) {
		printf("argc error!\n");
		exit(-1);
	}
    //打开目录
	if ((dirSrc = opendir(argv[1])) == NULL) {
		printf("open dir: %s error!\n", argv[1]);
		exit(-1);
	}
    if((dirDest = opendir(argv[2])) == NULL){
        stat(argv[1], &statbuf);
        utb.actime = statbuf.st_atime;
        utb.modtime = statbuf.st_mtime;
        //创建目录
        if (mkdir(argv[2], statbuf.st_mode) < 0) {
            printf("create dir: %s error!\n", argv[2]);
            exit(-1);
        }
    }else{
		printf("Filename duplication!\n");
		exit(-1);
	}
    //复制目录中的内容
	MyCopy(argv[1], argv[2]);
    utime(argv[2], &utb);
	printf("copy finished!\n");
	closedir(dirSrc);
	closedir(dirDest);
	return 0;
}

void MyCopy(const char *src, const char *dest){
    DIR *dir = NULL;                        // get the directory
    struct dirent *entry = NULL;
    struct stat statbuf;                    //stat用于存储文件各项属性信息
    struct stat directoryStat;
    struct utimbuf utb;
    char old_path[128], new_path[128];      //存放路径
    if((dir = opendir(src))==NULL){
        printf("file directory %s open error.\n", src);
        return ;
    }

    strcpy(old_path, src);
	strcpy(new_path, dest);

    while ((entry = readdir(dir))!=NULL){
        // ignore . .. file
        if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        //路径名拼接
		strcpy(new_path, dest);
		strcpy(old_path, src);
		strcat(old_path, "/");
		strcat(new_path, "/");
		strcat(old_path, entry->d_name);
		strcat(new_path, entry->d_name);
        // get the file information
		lstat(old_path, &statbuf);

        if(S_ISLNK(statbuf.st_mode)){ // old_file is a link file   
            printf("LinkFile: %s，正在拷贝...\n",old_path);
            copyLinkFile(old_path, new_path);
		}else if(S_ISDIR(statbuf.st_mode)){  //old_file is a directory
            printf("Director: %s，正在拷贝...\n",old_path);
            stat(old_path, &directoryStat);
            mkdir(new_path, directoryStat.st_mode);
            MyCopy(old_path, new_path);
            utb.actime = directoryStat.st_atime;
            utb.modtime = directoryStat.st_mtime;
            utime(new_path, &utb);
            printf("Directory: %s，拷贝完成\n",old_path);
        }else{  // old_file is a file
            printf("File: %s，正在拷贝...\n",old_path);
            copyFile(old_path, new_path);
        }
    }
    closedir(dir);
}
void copyLinkFile(const char *src, const char *dest){
    struct stat statbuf;        ///stat用于存储文件各项属性信息
    struct timeval tv[2];
	char buffer[1024];          //缓冲区，用于存放文件里的数据
    memset(buffer, 0, sizeof(buffer));
    lstat(src, &statbuf);
    readlink(src, buffer, 1024);
    if (symlink(buffer, dest) == -1) {
		printf("create link error!\n");
		return ;
	}
    printf("软连接文件复制成功\n");
	chmod(dest, statbuf.st_mode);
    tv[0].tv_sec = statbuf.st_atime;
	tv[0].tv_usec = 0;
	tv[1].tv_sec = statbuf.st_mtime;
	tv[1].tv_usec = 0;
	lutimes(dest, tv);
}
void copyFile(const char *src, const char *dest){
    struct stat statbuf;        ///stat用于存储文件各项属性信息
    struct utimbuf utb;
	int newFd;                  //新文件描述符
	int oldFd;                  //旧文件描述符
	int size = 0;               //每次从文件中读取的数据量
	char buffer[1024];          //缓冲区，用于存放文件里的数据
    memset(buffer, 0, sizeof(buffer));

    stat(src, &statbuf);
    utb.actime=statbuf.st_atime;
    utb.modtime=statbuf.st_mtime;
	if((newFd = creat(dest, statbuf.st_mode)) < 0){  //创建新文件，返回文件描述符
        printf("create file : %s error.\n", dest);
        return ;
    };
    //打开文件
	if ((oldFd = open(src, O_RDONLY)) < 0) {
		printf("open file: %s error!\n", src);
		return ;
	}

	//复制文件内容
	while ((size = read(oldFd, buffer, 1024)) > 0) {
		write(newFd, buffer, size);
	}
    utime(dest, &utb);
	///关闭文件
	close(oldFd);
	close(newFd);
}
