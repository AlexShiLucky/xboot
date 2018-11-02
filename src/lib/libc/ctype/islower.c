/*
 * libc/ctype/islower.c
 */

#include <ctype.h>

/* 判断是否是小写字母 */
int islower(int c)
{
	return ((unsigned)c - 'a') < 26;
}
EXPORT_SYMBOL(islower);
