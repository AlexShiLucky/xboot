/*
 * libc/environ/unsetenv.c
 */

#include <runtime.h>
#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <environ.h>

/* 删除一个环境变量 */
int unsetenv(const char * name)
{
    /* 获取当前运行环境的环境变量 */
	struct environ_t * environ = &(runtime_get()->__environ);
	struct environ_t * p;
	size_t len;
	const char * z;

	if(!name || !name[0])
		return -1;

	len = 0;
    /* 计算name长度,并不允许包含等号 */
	for(z = name; *z; z++)
	{
		len++;
		if(*z == '=')
			return -1;
	}

	if(!environ)
		return 0;

    /* 遍历所有环境变量节点 */
	for(p = environ->next; p != environ; p = p->next)
	{
        /* 匹配环境变量名称,并且随后的等号 */
		if(!strncmp(name, p->content, len) && (p->content[len] == '='))
		{
			p->next->prev = p->prev;
			p->prev->next = p->next;

			free(p->content);
			free(p);
			break;
		}
	}

	return 0;
}
EXPORT_SYMBOL(unsetenv);
