/*
 * kernel/core/device.c
 *
 * Copyright(c) 2007-2020 Jianjun Jiang <8192542@qq.com>
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
#include <xboot/device.h>

/* 全局设备链表 */
struct list_head __device_list;
/* 全局设备类型链表 */
struct list_head __device_head[DEVICE_TYPE_MAX_COUNT];
/* 全局设备哈希表 */
static struct hlist_head __device_hash[CONFIG_DEVICE_HASH_SIZE];
static spinlock_t __device_lock = SPIN_LOCK_INIT();
/* 设备通知链表 */
static struct notifier_chain_t __device_nc = NOTIFIER_CHAIN_INIT();

static const char* __device_name[] = {
#define Xdef(def, name)    name,
#include <xboot/device_table.h>
#undef Xdef
};

/* 根据设备名称获取device哈希表 */
static struct hlist_head * device_hash(const char * name)
{
	return &__device_hash[shash(name) % ARRAY_SIZE(__device_hash)];
}

/* 搜索设备kobj*/
static struct kobj_t * search_device_kobj(struct device_t * dev)
{
	struct kobj_t * kdevice;
	const char * name;

	if(!dev || !dev->kobj)
		return NULL;
    /* 在kobj下搜索(创建)device */
	kdevice = kobj_search_directory_with_create(kobj_get_root(), "device");
	if(!kdevice)
		return NULL;

    /* 根据设备类型获取设备名称关键字 */
    if (dev->type < DEVICE_TYPE_MAX_COUNT) {
        name = __device_name[dev->type];
    } else return NULL;

    /* 在kobj/device下创建name的设备路径 */
	return kobj_search_directory_with_create(kdevice, name);
}

static ssize_t device_write_suspend(struct kobj_t * kobj, void * buf, size_t size)
{
	struct device_t * dev = (struct device_t *)kobj->priv;

	if(strncmp(buf, dev->name, size) == 0)
		suspend_device(dev);
	return size;
}

static ssize_t device_write_resume(struct kobj_t * kobj, void * buf, size_t size)
{
	struct device_t * dev = (struct device_t *)kobj->priv;

	if(strncmp(buf, dev->name, size) == 0)
		resume_device(dev);
	return size;
}

/* 判断名称为name的设备是否存在 */
static bool_t device_exist(const char * name)
{
	struct device_t * pos;
	struct hlist_node * n;

	hlist_for_each_entry_safe(pos, n, device_hash(name), node)
	{
		if(strcmp(pos->name, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/* 根据设备主名和id号申请一个设备名称 */
char * alloc_device_name(const char * name, int id)
{
	char buf[256];

	if(id < 0)
		id = 0;
	do {
		snprintf(buf, sizeof(buf), "%s.%d", name, id++);
	} while(device_exist(buf));

	return strdup(buf);
}

/* 释放一个设备名称 */
void free_device_name(char * name)
{
	if(name)
		free(name);
}

/* 查找一个名称和类型都匹配的设备 */
struct device_t * search_device(const char * name, enum device_type_t type)
{
	struct device_t * pos;
	struct hlist_node * n;

	if(!name)
		return NULL;

	hlist_for_each_entry_safe(pos, n, device_hash(name), node)
	{
		if((pos->type == type) && (strcmp(pos->name, name) == 0))
			return pos;
	}
	return NULL;
}

/* 查找某类型的第一个设备 */
struct device_t * search_first_device(enum device_type_t type)
{
	if((type < 0) || (type >= ARRAY_SIZE(__device_head)))
		return NULL;
	return (struct device_t *)list_first_entry_or_null(&__device_head[type], struct device_t, head);
}

/* 注册一个设备 */
bool_t register_device(struct device_t * dev)
{
	irq_flags_t flags;

    /* 如果设备空或无名设备,则注册失败 */
	if(!dev || !dev->name)
		return FALSE;

    /* 如若设备类型为非已知设备类型,则注册失败 */
	if((dev->type < 0) || (dev->type >= ARRAY_SIZE(__device_head)))
		return FALSE;

    /* 如果设备名称已存在,则注册失败 */
	if(device_exist(dev->name))
		return FALSE;

    /* 在父kobj(xxxname)下挂载suspend文件kobj */
	kobj_add_regular(dev->kobj, "suspend", NULL, device_write_suspend, dev);
    /* 在父kobj(xxxname)下挂载resume文件kobj */
	kobj_add_regular(dev->kobj, "resume", NULL, device_write_resume, dev);
    /* 在kobj/device/devicetype下挂载xxxname */
	kobj_add(search_device_kobj(dev), dev->kobj);

	spin_lock_irqsave(&__device_lock, flags);
    /* 设备链表节点初始化 */
	init_list_head(&dev->list);
    /* 将设备链表节点挂接到全局设备链表中 */
	list_add_tail(&dev->list, &__device_list);
    /* 设备链表节点初始化 */
	init_list_head(&dev->head);
    /* 将设备链表节点挂接到按设备类型划分的全局设备链表中 */
	list_add_tail(&dev->head, &__device_head[dev->type]);
    /* 设备链表节点初始化 */
	init_hlist_node(&dev->node);
    /* 将设备链表节点挂接到全局设备哈希表中 */
	hlist_add_head(&dev->node, device_hash(dev->name));
	spin_unlock_irqrestore(&__device_lock, flags);
    /* 通知设备加入 */
	notifier_chain_call(&__device_nc, NOTIFIER_DEVICE_ADD, dev);

	return TRUE;
}

/* 注销一个设备 */
bool_t unregister_device(struct device_t * dev)
{
	irq_flags_t flags;

    /* 如果设备空或无名设备,则注销失败 */
	if(!dev || !dev->name)
		return FALSE;

    /* 如若设备类型为非已知设备类型,则注销失败 */
	if((dev->type < 0) || (dev->type >= ARRAY_SIZE(__device_head)))
		return FALSE;

	if(hlist_unhashed(&dev->node))
		return FALSE;

    /* 通知设备移除 */
	notifier_chain_call(&__device_nc, NOTIFIER_DEVICE_REMOVE, dev);
	spin_lock_irqsave(&__device_lock, flags);
    /* 移除设备链表节点 */
	list_del(&dev->list);
    /* 移除设备链表节点 */
	list_del(&dev->head);
    /* 移除设备链表节点 */
	hlist_del(&dev->node);
	spin_unlock_irqrestore(&__device_lock, flags);
    /* 在kobj/device/devicetype下移除devicename */
	kobj_remove(search_device_kobj(dev), dev->kobj);

	return TRUE;
}

/* 注册一个设备通知 */
bool_t register_device_notifier(struct notifier_t * n)
{
	return notifier_chain_register(&__device_nc, n);
}

/* 注销一个设备通知 */
bool_t unregister_device_notifier(struct notifier_t * n)
{
	return notifier_chain_unregister(&__device_nc, n);
}

/* 通知设备已挂起 */
void suspend_device(struct device_t * dev)
{
	if(dev)
	{
	    /* 先通知设备已挂起 */
		notifier_chain_call(&__device_nc, NOTIFIER_DEVICE_SUSPEND, dev);
        /* 再挂起设备 */
		if(dev->driver && dev->driver->suspend)
			dev->driver->suspend(dev);
	}
}

/* 通知设备已释放 */
void resume_device(struct device_t * dev)
{
	if(dev)
	{
	    /* 先释放设备 */
		if(dev->driver && dev->driver->resume)
			dev->driver->resume(dev);
        /* 再通知设备已释放*/
		notifier_chain_call(&__device_nc, NOTIFIER_DEVICE_RESUME, dev);
	}
}

/* 初始化全局设备链表、全局设备类型链表、全局设备哈希表 */
static __init void device_pure_init(void)
{
	int i;

	init_list_head(&__device_list);
	for(i = 0; i < ARRAY_SIZE(__device_head); i++)
		init_list_head(&__device_head[i]);
	for(i = 0; i < ARRAY_SIZE(__device_hash); i++)
		init_hlist_head(&__device_hash[i]);
}
pure_initcall(device_pure_init);
