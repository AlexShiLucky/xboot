/*
 * libc/environ/putenv.c
 */

#include <string.h>
#include <malloc.h>
#include <environ.h>
#include <xboot/module.h>

/* 加入一个环境变量键值对字符串,len表示"name="的长度 */
int __put_env(char * str, size_t len, int overwrite)
{
	struct environ_t * environ = &__environ;
	struct environ_t * env;
	struct environ_t * p;

	if(!environ)
		return -1;

    /* 遍历所有环境变量节点 */
	for(p = environ->next; p != environ; p = p->next)
	{
		if(p->content && !strncmp(p->content, str, len))
		{
			if(!overwrite) /* 不覆盖 */
			{
				free(str);
			}
			else            /* 覆盖 */
			{
				free(p->content);
				p->content = str;
			}
			return 0;
		}
	}

    /* 申请一个新的环境变量节点 */
	env = malloc(sizeof(struct environ_t));
	if(!env)
		return -1;

    /* 在环境变量链表中插入新节点 */
	env->content = str;
	env->prev = environ->prev;
	env->next = environ;
	environ->prev->next = env;
	environ->prev = env;

	return 0;
}

/* 加入一个环境变量字符串 */
int putenv(const char * str)
{
	char * s;
	const char * e, * z;

	if(!str)
		return -1;

	e = NULL;
	for(z = str; *z; z++)
	{
		if(*z == '=')
			e = z;
	}

	if(!e)
		return -1;

	s = strdup(str);
	if(!s)
		return -1;

	return __put_env(s, e - str, 1);
}
EXPORT_SYMBOL(putenv);
