/*
 * kernel/command/cmd-ps.c
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

#include <command/command.h>

static void usage(void)
{
	printf("usage:\r\n");
	printf("    ps\r\n");
}

static int do_ps(int argc, char ** argv)
{
	struct scheduler_t * sched;
	struct task_t * pos, * n;
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
	{
		sched = __sched[i];

		printf("CPU%d:\r\n", i);

		pos = sched->running;
		printf(" %p %-8s %3d %20lld %s\r\n", pos->func, "Running", pos->nice, pos->time, pos->path ? pos->path : "");

		rbtree_postorder_for_each_entry_safe(pos, n, &sched->ready.rb_root, node)
		{
			printf(" %p %-8s %3d %20lld %s\r\n", pos->func, "Ready", pos->nice, pos->time, pos->path ? pos->path : "");
		}

		list_for_each_entry_safe(pos, n, &sched->suspend, list)
		{
			printf(" %p %-8s %3d %20lld %s\r\n", pos->func, "Suspend", pos->nice, pos->time, pos->path ? pos->path : "");
		}
	}
	return 0;
}

static struct command_t cmd_ps = {
	.name	= "ps",
	.desc	= "report a snapshot of the current processes",
	.usage	= usage,
	.exec	= do_ps,
};

static __init void ps_cmd_init(void)
{
	register_command(&cmd_ps);
}

static __exit void ps_cmd_exit(void)
{
	unregister_command(&cmd_ps);
}

command_initcall(ps_cmd_init);
command_exitcall(ps_cmd_exit);
