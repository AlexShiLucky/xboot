/*
 * driver/input/input.c
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
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
#include <input/input.h>

/* 根据名称搜索一个输入设备 */
struct input_t * search_input(const char * name)
{
	struct device_t * dev;

	dev = search_device(name, DEVICE_TYPE_INPUT);
	if(!dev)
		return NULL;
	return (struct input_t *)dev->priv;
}

/* 注册一个输入设备 */
bool_t register_input(struct device_t ** device, struct input_t * input)
{
	struct device_t * dev;

	if(!input || !input->name)
		return FALSE;

	dev = malloc(sizeof(struct device_t));
	if(!dev)
		return FALSE;

	dev->name = strdup(input->name);
	dev->type = DEVICE_TYPE_INPUT;
	dev->driver = NULL;
	dev->priv = input;
	dev->kobj = kobj_alloc_directory(dev->name);

	if(!register_device(dev))
	{
		kobj_remove_self(dev->kobj);
		free(dev->name);
		free(dev);
		return FALSE;
	}

	if(device)
		*device = dev;
	return TRUE;
}

/* 注销一个输入设备 */
bool_t unregister_input(struct input_t * input)
{
	struct device_t * dev;

	if(!input || !input->name)
		return FALSE;

	dev = search_device(input->name, DEVICE_TYPE_INPUT);
	if(!dev)
		return FALSE;

	if(!unregister_device(dev))
		return FALSE;

	kobj_remove_self(dev->kobj);
	free(dev->name);
	free(dev);
	return TRUE;
}

/* 输入设备io控制接口调用 */
int input_ioctl(struct input_t * input, int cmd, void * arg)
{
	if(input && input->ioctl)
		return input->ioctl(input, cmd, arg);
	return -1;
}
