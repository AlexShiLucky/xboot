/*
 * init/runtime.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xfs/xfs.h>
#include <runtime.h>

/* 当前运行环境指针 */
static struct runtime_t * __current_runtime = NULL;

/* 获取当前运行环境 */
struct runtime_t * runtime_get(void)
{
	return __current_runtime;
}

/* 创建新运行环境并保存先前运行环境 */
void runtime_create_save(struct runtime_t * rt, const char * path, struct runtime_t ** r)
{
	if(!rt)
		return;
    /* 返回先前运行环境 */
	if(r)
		*r = __current_runtime;
    /* 设置当前运行环境 */
	__current_runtime = rt;

	rt->__errno = 0;

	rt->__seed[0] = 1;
	rt->__seed[1] = 1;
	rt->__seed[2] = 1;

    /* 初始化当前运行环境的环境变量链表 */
	rt->__environ.content = "";
	rt->__environ.next = &(rt->__environ);
	rt->__environ.prev = &(rt->__environ);

    /* 读取运行环境分配stdin */
	rt->__stdin = __file_alloc(0);
    /* 读取运行环境分配stdout */
	rt->__stdout = __file_alloc(1);
    /* 读取运行环境分配stderr */
	rt->__stderr = __file_alloc(2);

    /* 给当前运行环境申请一个事件块 */
	rt->__event_base = __event_base_alloc();
    /* 给当前运行环境申请一个xfs上下文 */
	rt->__xfs_ctx = __xfs_alloc(path);
}

/* 销毁当前运行环境并恢复先前运行环境 */
void runtime_destroy_restore(struct runtime_t * rt, struct runtime_t * r)
{
	if(!rt)
		return;

	if(rt->__xfs_ctx)
		__xfs_free(rt->__xfs_ctx);

	if(rt->__event_base)
		__event_base_free(rt->__event_base);

	if(rt->__stderr)
		fclose(rt->__stderr);

	if(rt->__stdout)
		fclose(rt->__stdout);

	if(rt->__stdin)
		fclose(rt->__stdin);

	if(r)
		__current_runtime = r;
}
