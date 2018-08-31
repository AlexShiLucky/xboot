/*
 * libc/string/strsep.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Extract token from string
 */
/* 根据分隔符ct分解字符串s, 函数返回分隔符前部分, s返回分隔符后部分 */
char * strsep(char ** s, const char * ct)
{
	char * sbegin = *s;
	char * end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;

	return sbegin;
}
EXPORT_SYMBOL(strsep);
