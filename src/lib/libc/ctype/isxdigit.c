/*
 * libc/ctype/isxdigit.c
 */

#include <ctype.h>

/* 判断是否为十六进制数 */
int isxdigit(int c)
{
	return isdigit(c) || (((unsigned)c | 32) - 'a' < 6);
}
EXPORT_SYMBOL(isxdigit);
