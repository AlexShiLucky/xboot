/*
 * kernel/core/subsys.c
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

extern unsigned char __romdisk_start;
extern unsigned char __romdisk_end;

/* 子系统romdisk初始化 */
static void subsys_init_romdisk(void)
{
	char json[256];
	int length;

    /* json = "{romdisk@0:{address:xxxxxxxx,size:yyyyyyyy}}" */
	length = sprintf(json,
		"{\"romdisk@0\":{\"address\":\"%lld\",\"size\":\"%lld\"}}",
		(unsigned long long)(&__romdisk_start),
		(unsigned long long)(&__romdisk_end - &__romdisk_start));
    /* 探测romdisk设备 */
	probe_device(json, length);
}

/* 子系统根文件系统初始化 */
static void subsys_init_rootfs(void)
{
    /* mount块设备romdisk.0到根目录下的文件系统cpiofs */
	mount("romdisk.0", "/", "cpiofs", 0); chdir("/");
    /* mount /sys文件系统sysfs*/
	mount(NULL, "/sys", "sysfs", 0);
	mount(NULL, "/storage" , "ramfs", 0);
	mount(NULL, "/private" , "ramfs", 0);
	mkdir("/private/application", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	mkdir("/private/userdata", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}

/* 子系统设备树初始化 */
static void subsys_init_dt(void)
{
	char path[64];
	char * json;
	int fd, n, len = 0;

	json = malloc(SZ_1M);
	if(!json)
		return;

    /* 获取机器配置文件路径 */
	sprintf(path, "/boot/%s.json", get_machine()->name);
	if((fd = open(path, O_RDONLY, (S_IRUSR | S_IRGRP | S_IROTH))) > 0)
	{
	    for(;;)
	    {
	        n = read(fd, (void *)(json + len), SZ_512K);
	        if(n <= 0)
	        	break;
			len += n;
	    }
	    close(fd);
        /* 探测json配置文件中的设备 */
	    probe_device(json, len);
	}
	free(json);
}

/* 子系统初始化 */
static __init void subsys_init(void)
{
    /* 初始化romdisk */
	subsys_init_romdisk();
    /* 初始化根文件系统 */
	subsys_init_rootfs();
    /* 初始化设备树 */
	subsys_init_dt();
}
subsys_initcall(subsys_init);
