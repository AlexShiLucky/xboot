/*
 * libc/stdio/freopen.c
 */

#include <stdio.h>
#include <xboot/module.h>

/* 重新打开文件 */
FILE * freopen(const char * path, const char * mode, FILE * f)
{
	/* Not support redirect yet */
	if(f)
		fclose(f);
	return fopen(path, mode);
}
EXPORT_SYMBOL(freopen);
