/*
 * libc/stdio/__stdio.c
 */

#include <xboot/module.h>
#include <console/console.h>
#include <stdio.h>

/* 标准输入文件指针 */
static FILE * __stdin = NULL;
/* 标准输出文件指针 */
static FILE * __stdout = NULL;
/* 标准错误文件指针 */
static FILE * __stderr = NULL;

/* 终端标准输入读 */
static ssize_t __tty_stdin_read(FILE * f, unsigned char * buf, size_t size)
{
	return console_stdin_read(buf, size);
}

/* 终端标准输出写 */
static ssize_t __tty_stdout_write(FILE * f, const unsigned char * buf, size_t size)
{
	return console_stdout_write(buf, size);
}

/* 终端标准错误写 */
static ssize_t __tty_stderr_write(FILE * f, const unsigned char * buf, size_t size)
{
	return console_stderr_write(buf, size);
}

/* 终端空读 */
static ssize_t __tty_null_read(FILE * f, unsigned char * buf, size_t size)
{
	return 0;
}

/* 终端空写 */
static ssize_t __tty_null_write(FILE * f, const unsigned char * buf, size_t size)
{
	return 0;
}

/* 终端空seek */
static fpos_t __tty_null_seek(FILE * f, fpos_t off, int whence)
{
	return 0;
}

/* 终端空关闭 */
static int __tty_null_close(FILE * f)
{
	return 0;
}

/* 文件读取 */
static ssize_t __file_read(FILE * f, unsigned char * buf, size_t size)
{
	return vfs_read(f->fd, (void *)buf, size);
}

/* 文件写入 */
static ssize_t __file_write(FILE * f, const unsigned char * buf, size_t size)
{
	return vfs_write(f->fd, (void *)buf, size);
}

/* 文件seek */
static fpos_t __file_seek(FILE * f, fpos_t off, int whence)
{
	return vfs_lseek(f->fd, off, whence);
}

/* 文件关闭 */
static int __file_close(FILE * f)
{
	return vfs_close(f->fd);
}

/*
 * file alloc
 * 根据文件描述符申请文件操作块
 */
FILE * __file_alloc(int fd)
{
	FILE * f;

    /* 分配文件操作块内存 */
	f = malloc(sizeof(FILE));
	if(!f)
		return 0;

	if(fd == 0)         // fd=0,为标准输入
	{
		f->fd = fd;

        /* 标准输入挂接终端标准输入读 */
		f->read = __tty_stdin_read;
        /* 标准输入无写操作 */
		f->write = __tty_null_write;
        /* 标准输入无seek操作 */
		f->seek = __tty_null_seek;
        /* 标准输入无关闭操作 */
		f->close = __tty_null_close;

        /* 分配一个BUFSIZ长度的读FIFO */
		f->fifo_read = fifo_alloc(BUFSIZ);
        /* 分配一个BUFSIZ长度的写FIFO */
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
        /* 标准输入挂接无冲刷 */
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;   // 标准输入为行缓冲模式
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 1)    // fd=1,为标准输出
	{
		f->fd = fd;

        /* 标准输出无读操作 */
		f->read = __tty_null_read;
        /* 标准输出挂接终端标准输出写 */
		f->write = __tty_stdout_write;
        /* 标准输出无seek操作 */
		f->seek = __tty_null_seek;
        /* 标准输出无关闭操作 */
		f->close = __tty_null_close;

        /* 分配一个BUFSIZ长度的读FIFO */
		f->fifo_read = fifo_alloc(BUFSIZ);
        /* 分配一个BUFSIZ长度的写FIFO */
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
        /* 标准输出挂接无冲刷 */
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOLBF;   // 标准输出为行缓冲模式
		f->error = 0;
		f->eof = 0;
	}
	else if(fd == 2)    // fd=2,为标准错误输出
	{
		f->fd = fd;

        /* 标准错误输出无读操作 */
		f->read = __tty_null_read;
        /* 标准错误输出挂接终端标准错误写 */
		f->write = __tty_stderr_write;
        /* 标准错误输出无seek操作 */
		f->seek = __tty_null_seek;
        /* 标准错误输出无关闭操作 */
		f->close = __tty_null_close;

        /* 分配一个BUFSIZ长度的读FIFO */
		f->fifo_read = fifo_alloc(BUFSIZ);
        /* 分配一个BUFSIZ长度的写FIFO */
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
        /* 标准错误输出挂接无冲刷 */
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IONBF;   // 标准错误输出为无缓冲模式
		f->error = 0;
		f->eof = 0;
	}
	else                // 其他文件描述符则为文件的读写
	{
		f->fd = fd;

        /* 挂接文件读 */
		f->read = __file_read;
        /* 挂接文件写 */
		f->write = __file_write;
        /* 挂接文件seek */
		f->seek = __file_seek;
        /* 挂接文件关闭 */
		f->close = __file_close;

        /* 分配一个BUFSIZ长度的读FIFO */
		f->fifo_read = fifo_alloc(BUFSIZ);
        /* 分配一个BUFSIZ长度的写FIFO */
		f->fifo_write = fifo_alloc(BUFSIZ);

		f->buf = malloc(BUFSIZ);
		f->bufsz = BUFSIZ;
        /* 文件读写冲刷挂接无冲刷 */
		f->rwflush = &__stdio_no_flush;

		f->pos = 0;
		f->mode = _IOFBF;   // 文件读写为完全缓冲模式
		f->error = 0;
		f->eof = 0;
	}

	return f;
}

/* 获取标准输入文件指针 */
FILE * __stdio_get_stdin(void)
{
	if(!__stdin)
		__stdin = __file_alloc(0);
	return __stdin;
}
EXPORT_SYMBOL(__stdio_get_stdin);

/* 获取标准输出文件指针 */
FILE * __stdio_get_stdout(void)
{
	if(!__stdout)
		__stdout = __file_alloc(1);
	return __stdout;
}
EXPORT_SYMBOL(__stdio_get_stdout);

/* 获取标准错误文件指针 */
FILE * __stdio_get_stderr(void)
{
	if(!__stderr)
		__stderr = __file_alloc(2);
	return __stderr;
}
EXPORT_SYMBOL(__stdio_get_stderr);
