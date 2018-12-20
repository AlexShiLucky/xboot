/*
 * libc/environ/unsetenv.c
 */

#include <errno.h>
#include <string.h>
#include <malloc.h>
#include <environ.h>
#include <xboot/module.h>

/* 删除一个环境变量 */
int unsetenv(const char * name)
{
	struct environ_t * environ = &__environ;
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

	if(!environ || !environ->content)
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
