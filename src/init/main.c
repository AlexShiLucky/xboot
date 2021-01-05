/*
 * init/main.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
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

/* xboot入口函数,从start.S跳转过来 */
void xboot_main(void)
{
	/* Do initial memory - 初始化内存 */
	do_init_mem();

	/* Do initial scheduler - 初始化调度器 */
	do_init_sched();

	/* Do initial vfs - 初始化虚拟文件系统 */
	do_init_vfs();

	/* Do initial calls - 初始化表调用 */
	do_initcalls();

	/* Do initial setting - 初始化设置 */
	do_init_setting();

	/* Do show logo - 显示logo */
	do_show_logo();

	/* Do play audio */
	do_play_audio();

	/* Do auto mount */
	do_auto_mount();

	/* Do auto boot - 调用init.c中的__do_autoboot */
	do_auto_boot();

#if defined(CONFIG_SHELL_TASK) && (CONFIG_SHELL_TASK > 0)
	/* Create and resume shell task */
	task_resume(task_create(scheduler_self(), "shell", shell_task, NULL, 0, 0));
#endif

	/* Scheduler loop */
	scheduler_loop();

	/* Do exit calls */
	do_exitcalls();
}
