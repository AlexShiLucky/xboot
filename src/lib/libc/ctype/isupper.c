/*
 * libc/ctype/isupper.c
 */

#include <ctype.h>

/* 判断是否为大写字母 */
int isupper(int c)
{
	return ((unsigned)c - 'A') < 26;
}
EXPORT_SYMBOL(isupper);
