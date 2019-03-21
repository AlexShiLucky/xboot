/*
 * libc/environ/getenv.c
 */

#include <stddef.h>
#include <string.h>
#include <environ.h>
#include <xboot/module.h>

/* 获取环境变量值 */
char * getenv(const char * name)
{
	struct environ_t * xenv = &__xenviron;
	struct environ_t * p;
	int len;

	if(!xenv || !xenv->content)
		return NULL;

	len = strlen(name);
    /* 遍历环境变量链表,查找name的环境变量 */
	for(p = xenv->next; p != xenv; p = p->next)
	{
	    /* 环境变量存储格式"name=value" */
		if(!strncmp(name, p->content, len) && (p->content[len] == '='))
			return p->content + (len + 1);
	}
	return NULL;
}
EXPORT_SYMBOL(getenv);
