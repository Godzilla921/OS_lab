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


# define bufferSize 1024		// 缓冲区大小
# define nameLength	128			// 文件(文件夹)路径名长度

// 递归函数，用以进行区别文件类型
void MyCopy(const char *src, const char *dest);
// 软连接文件复制
void copyLinkFile(const char *src, const char *dest);
// 文件复制
void copyFile(const char *src, const char *dest);
// 目录复制
void copyDirectory(const char *src, const char *dest);

int main(int argc, char** argv){
	struct stat dirstat;	// dirstat 用于存储目录文件的属性信息
	struct utimbuf utb;		// 源目录访问与修改时间
	DIR *dirSrc = NULL;		// 源文件目录指针
	DIR *dirDest = NULL;    // 目标文件目录指针

	if (argc != 3) {            // 若命令行参数不为 3 ， 输入格式错误
		printf("argc error!\n");
		exit(-1);
	}

	if ((dirSrc = opendir(argv[1])) == NULL) {  // 源目录打开失败
		printf("open srcDir: %s error!\n", argv[1]);
		exit(-1);
	}
	if((dirDest = opendir(argv[2])) == NULL){   // 若目标目录不存在
		stat(argv[1], &dirstat);                // 得到源目录的文件属性
		utb.actime = dirstat.st_atime;          
		utb.modtime = dirstat.st_mtime;
		// 使用mkdir()创建目录, 失败则返回-1
		if (mkdir(argv[2], dirstat.st_mode) < 0) {
			printf("create dir: %s error!\n", argv[2]);
			exit(-1);
		}
	}else{  
		// 若dirDest!=NULL 则 dest 已存在，文件夹复制失败
		printf("%s is existded!\n", argv[2]);
		exit(-1);
	}
	// 开始递归复制 argv[1] 路径下的所有文件
	MyCopy(argv[1], argv[2]);
	 // 修改 目录 argv[2] 的访问与修改时间
	utime(argv[2], &utb);
	// 文件复制成功
	printf("copy finished!\n");
	// 关闭源目录文件与目标目录文件指针
	closedir(dirSrc);
	closedir(dirDest);
	return 0;
}

void MyCopy(const char *src, const char *dest){
	DIR *dir = NULL;                        // 目录文件指针
	struct dirent *entry = NULL;            // 目录下的子文件(子目录)指针
	struct stat filestat;                   // filestat 获取当前目录的文件属性
	 // 子文件(文件夹)路径名称
	char child_src_path[nameLength], child_dest_path[nameLength];     
	
	if((dir = opendir(src))==NULL){         // 获取当前目录文件的指针
		printf("file directory %s open error.\n", src);
		return ;
	}

	while ((entry = readdir(dir))!=NULL){   // 获取一个子文件/文件夹
		// 忽略 ./ 当前文件夹 ../ 父级文件夹
		if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			// 路径名拼接 基路径/文件名（文件夹名）
			strcpy(child_src_path, src);             // 赋值子文件/文件夹的基路径
			strcpy(child_dest_path, dest);
			strcat(child_src_path, "/");             // 加上文件连接符 /
			strcat(child_dest_path, "/");
			strcat(child_src_path, entry->d_name);   // 加上文件名(文件夹名)
			strcat(child_dest_path, entry->d_name);
			// 子文件 可能有三种情况: 目录、软连接文件、文件
			// 使用lstat 获取文件的属性可以满足三种类型的文件，而stat()无法获得软连接文件的属性
			lstat(child_src_path, &filestat);            
		// 根据子不同的文件类型进行不同的处理
		if(S_ISLNK(filestat.st_mode)){		// 子文件为 软连接文件
			// 进行软连接复制
			copyLinkFile(child_src_path, child_dest_path);
		}else if(S_ISDIR(filestat.st_mode)){	// 子文件为目录文件，进行目录复制，同时递归复制该目录下的所有文件及文件夹
			// 进行目录复制
			copyDirectory(child_src_path, child_dest_path);
		}else{					// 该子文件为文件
			// 进行软连接文件复制
			copyFile(child_src_path, child_dest_path);
        	}
	}
	// 关闭该目录文件指针
	closedir(dir);
}
void copyLinkFile(const char *src, const char *dest){
	printf("LinkFile: %s，正在拷贝...\n",src);
	struct stat filestat;           // filestat 用于软连接文件各项属性信息
	struct timeval tv[2];           // tv 为设置软链接文件访问与修改时间的数据结构
	char path[nameLength];          // 存储软连接文件的链接信息，用于新建软链接文件
	memset(path, 0, nameLength);  	// 缓冲区清零
	// 获取 源文件的链接属性信息
	lstat(src, &filestat);
	// 读取符号链接文件本身的信息到缓冲区 buffer 中;执行成功则传符号连接所指的文件路径字符串
	readlink(src, path, nameLength);
	// symlink() 对于已有的文件 path 建立一个名为 dest 的符号连接。
	if (symlink(path, dest) == -1) {   // 成功返回0 失败返回-1
		printf("create link: %s error!\n", src);
		return ;
	}
	printf("软连接文件复制成功\n");
	// chmod改变文件的改变文件的权限
	chmod(dest, filestat.st_mode);
	// 设置新建链接文件的属性
	tv[0].tv_sec = filestat.st_atime;       // 秒
	tv[0].tv_usec = 0;                      // 微秒
	tv[1].tv_sec = filestat.st_mtime;
	tv[1].tv_usec = 0;
	// 更新目标文件的访问与修改时间与源文件一致
	lutimes(dest, tv);
}
void copyFile(const char *src, const char *dest){
	printf("File: %s，正在拷贝...\n",src);
	struct stat filestat;           // filestat用于存储文件各项属性信息
	struct utimbuf utb;             // 访问与修改时间结构体
	int srcFd;                      // 源文件描述符
	int destFd;                     // 目标文件描述符
	int size = 0;                   // 实际每次循环从文件中读取的数据量
	char buffer[bufferSize]; 		// 缓冲区，用于存放文件里的数据
	
	memset(buffer, 0, bufferSize);  // 初始化读文件缓冲区
	stat(src, &filestat);           // 获取源文件的属性
	
	// 赋值源文件的访问与修改时间
	utb.actime=filestat.st_atime;
	utb.modtime=filestat.st_mtime;
	
	// 新建一个文件，并得到该文件的描述符
	if((destFd = creat(dest, filestat.st_mode)) < 0){ 
		printf("create file : %s error.\n", dest);
		return ;
	};
	
	// 打开源文件的，并得到源文件描述符
	if ((srcFd = open(src, O_RDONLY)) < 0) {
		printf("open file: %s error!\n", src);
		return ;
	}

	// 从源文件中读取内容到缓冲区buffer中，read返回实际读出的字节数,<=0则文件读取完毕
	while ((size = read(srcFd, buffer, bufferSize)) > 0) {
        	// 将缓冲区的内容写入目标文件中
		write(destFd, buffer, size);
	}
	// 更新目标文件的访问与修改时间与源文件一致
	utime(dest, &utb);
	// 关闭源文件与目标文件
	close(srcFd);
	close(destFd);
}
void copyDirectory(const char *src, const char *dest){
	printf("Director: %s，正在拷贝...\n",src);
	struct stat filestat;       // 源目录文件的属性
	struct utimbuf utb;         // 源目录文件的访问时间与修改时间   
	// 获取源目录文件的属性
	stat(src, &filestat);
	// 创建新的目录文件 child_dest_path
	mkdir(dest, filestat.st_mode);
	// 复制目录文件的 访问与修改时间
	utb.actime = filestat.st_atime;
	utb.modtime = filestat.st_mtime;
	// 递归进入该目录，进行子文件与子文件夹的复制
	MyCopy(src, dest);
	// 更新新建目录文件的时间
	utime(dest, &utb);
	// 复制成功
	printf("Directory: %s，拷贝完成\n",src);
}
