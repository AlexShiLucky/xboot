/*
 * libc/environ/getenv.c
 */

#include <runtime.h>
#include <stddef.h>
#include <string.h>
#include <environ.h>

/* 获取环境变量值 */
char * getenv(const char * name)
{
    /* 获取当前运行环境的环境变量 */
    struct environ_t * environ = &(runtime_get()->__environ);
    struct environ_t * p;
    int len;

    if(!environ)
        return NULL;

    len = strlen(name);
    /* 遍历环境变量链表,查找name的环境变量 */
    for(p = environ->next; p != environ; p = p->next)
    {
        /* 环境变量存储格式"name=value" */
        if(!strncmp(name, p->content, len) && (p->content[len] == '='))
        {
            return p->content + (len + 1);
        }
    }

    return NULL;
}
EXPORT_SYMBOL(getenv);
