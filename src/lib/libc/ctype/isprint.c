/*
 * libc/ctype/isprint.c
 */

#include <ctype.h>
#include <xboot/module.h>

/* 判断是否为可打印字符 */
int isprint(int c)
{
	return ((unsigned)c - 0x20) < 0x5f;
}
EXPORT_SYMBOL(isprint);
