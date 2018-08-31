/*
 * libc/string/strpbrk.c
 */

#include <types.h>
#include <stddef.h>
#include <string.h>

/*
 * Finds in a string the first occurrence of a byte/wchar_t in a set
 */
/* 比较字符串s1和s2中是否有相同的字符，如果有,则返回该字符在s1中的位置的指针 */
char * strpbrk(const char * s1, const char * s2)
{
	const char * sc1, * sc2;

	for (sc1 = s1; *sc1 != '\0'; ++sc1)
	{
		for (sc2 = s2; *sc2 != '\0'; ++sc2)
		{
			if (*sc1 == *sc2)
				return (char *)sc1;
		}
	}

	return NULL;
}
EXPORT_SYMBOL(strpbrk);
