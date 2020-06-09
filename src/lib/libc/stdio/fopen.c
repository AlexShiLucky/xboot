/*
 * libc/stdio/fopen.c
 */

#include <errno.h>
#include <stdio.h>
#include <vfs/vfs.h>
#include <xboot/module.h>

/* 打开文件 */
FILE * fopen(const char * path, const char * mode)
{
	FILE * f;
	int flags = O_RDONLY;
	int plus = 0;
	int fd;

	if((path == NULL) || (*path == '\0'))
	{
		errno = EINVAL;
		return NULL;
	}

    /* 解析文件打开模式字符串内容 */
	while(*mode)
	{
		switch(*mode++)
		{
		case 'r':
			flags = O_RDONLY;   // r: 只读
			break;
		case 'w':
			flags = O_WRONLY | O_CREAT | O_TRUNC;   // w: 只写
			break;
		case 'a':
			flags = O_WRONLY | O_CREAT | O_APPEND;  // a: 只写|追加
			break;
		case '+':
			plus = 1;   // 可读可写
			break;
		}
	}

	if(plus)
		flags = (flags & ~(O_RDONLY | O_WRONLY)) | O_RDWR;

    /* 打开path路径下的文件,返回文件描述符 */
	fd = vfs_open(path, flags, 0644);
	if(fd < 0)
		return NULL;

    /* 根据返回的文件描述符分配文件操作块 */
	f = __file_alloc(fd);
	if(!f)
	{
		vfs_close(fd);
		return NULL;
	}
	f->pos = vfs_lseek(f->fd, 0, VFS_SEEK_CUR);

	return f;
}
EXPORT_SYMBOL(fopen);
