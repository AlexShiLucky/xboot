/*
 * init/main.c
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

#include <xboot.h>
#include <init.h>
#include <dma/dma.h>
#include <shell/shell.h>

/* xboot入口函数,从start.S跳转过来 */
int xboot_main(int argc, char * argv[])
{
	struct task_t * task;

	/* Do initial mem pool - 初始化内存池 */
	do_init_mem_pool();

	/* Do initial dma pool - 初始化DMA池 */
	do_init_dma_pool();

	/* Do initial event */
	do_init_event();

	/* Do initial vfs - 初始化虚拟文件系统 */
	do_init_vfs();

	/* Do all initial calls - 初始化表调用 */
	do_initcalls();

	/* Do show logo - 显示logo */
	do_showlogo();

	/* Do auto boot - 调用init.c中的__do_autoboot */
	do_autoboot();

	/* Create shell task */
	task = task_create(scheduler_self(), "shell", shell_task, NULL, 0, 0);

	/* Resume shell task */
	task_resume(task);

	/* Scheduler loop */
	scheduler_loop();

	/* Do all exit calls */
	do_exitcalls();

	/* Xboot return */
	return 0;
}
