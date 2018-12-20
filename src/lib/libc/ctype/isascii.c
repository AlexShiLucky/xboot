/*
 * libc/ctype/isascii.c
 */

#include <ctype.h>
#include <xboot/module.h>

/* 判断数字c是否为ascii字符 */
int isascii(int c)
{
    /* 高位是否为0 */
	return !(c & ~0x7f);
}
EXPORT_SYMBOL(isascii);
