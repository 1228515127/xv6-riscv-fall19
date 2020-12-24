#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


/*
	将路径格式化为文件名
	得到最后一个'/'后面的文件名
	如./sleep
	得到sleep
*/
char* fmt_name(char *path) {
	static char buf[DIRSIZ + 1];
	char *p;

	// Find first character after last slash.
	for (p = path + strlen(path); p >= path && *p != '/'; p--);
	p++;
	memmove(buf, p, strlen(p) + 1);
	return buf;
}
/*
	在某路径中查找某文件
*/
void find(char *path, char *findName) {
	int fd;
	struct stat st;
	if ((fd = open(path, O_RDONLY)) < 0) {
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}
	if (fstat(fd, &st) < 0) {
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}
	char buf[512], *p;
	struct dirent de;
	switch (st.type) {
	case T_FILE:
		if (strcmp(fmt_name(path), findName) == 0) {
			printf("%s\n", path);
		}
		break;
	case T_DIR:
		if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
			printf("find: path too long\n");
			break;
		}
		strcpy(buf, path);
		p = buf + strlen(buf);
		*p++ = '/';
		while (read(fd, &de, sizeof(de)) == sizeof(de)) {
			//printf("de.name:%s, de.inum:%d\n", de.name, de.inum);
			//由注释的printf
			//应该跳过下面这些
			if (de.inum == 0 || de.inum == 1 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
				continue;
			memmove(p, de.name, strlen(de.name));
			p[strlen(de.name)] = 0;
			find(buf, findName);
		}
		break;
	}
	close(fd);
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("arguments are inadequate\n");
		exit();
	}
	find(argv[1], argv[2]);
	exit();
}