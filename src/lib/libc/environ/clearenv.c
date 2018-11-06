/*
 * libc/environ/clearenv.c
 */

#include <environ.h>

/* 清除所有环境变量 */
int clearenv(void)
{
	/* 获取当前运行环境的环境变量 */
	struct environ_t * environ = &__environ;
	struct environ_t * p, * q;

	if (!environ || !environ->content)
		return -1;

    /* 遍历所有环境变量节点,并逐一删除 */
	for(p = environ->next; p != environ;)
	{
		q = p;
		p = p->next;

		q->next->prev = q->prev;
		q->prev->next = q->next;
		free(q->content);
		free(q);
	}
	return 0;
}
EXPORT_SYMBOL(clearenv);
