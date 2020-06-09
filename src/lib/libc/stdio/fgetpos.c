/*
 * libc/stdio/fgetpos.c
 */

#include <stdio.h>
#include <xboot/module.h>

/* 获取文件游标位置 */
int fgetpos(FILE * f, fpos_t * pos)
{
	*pos = f->pos;
	return 0;
}
EXPORT_SYMBOL(fgetpos);
