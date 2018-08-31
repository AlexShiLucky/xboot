/*
 * kernel/core/driver.c
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

#include <xboot/driver.h>

/* 驱动哈希链表 */
static struct hlist_head __driver_hash[CONFIG_DRIVER_HASH_SIZE];
static spinlock_t __driver_lock = SPIN_LOCK_INIT();

/* 根据驱动名称获取device哈希表 */
static struct hlist_head * driver_hash(const char * name)
{
	unsigned char * p = (unsigned char *)name;
	unsigned int seed = 131;
	unsigned int hash = 0;

	while(*p)
	{
		hash = hash * seed + (*p++);
	}
	return &__driver_hash[hash % ARRAY_SIZE(__driver_hash)];
}

/* 搜索驱动kobj */
static struct kobj_t * search_class_driver_kobj(void)
{
    /* 在kobj下搜索(创建)class */
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
    /* 在kobj/class下创建driver路径 */
	return kobj_search_directory_with_create(kclass, "driver");
}

/* 驱动写探针 */
static ssize_t driver_write_probe(struct kobj_t * kobj, void * buf, size_t size)
{
	struct driver_t * drv = (struct driver_t *)kobj->priv;
	struct dtnode_t n;
	json_value * v;
	char * p;
	int i;

	if(buf && (size > 0))
	{
		v = json_parse(buf, size);
		if(v && (v->type == json_object))
		{
			for(i = 0; i < v->u.object.length; i++)
			{
				p = (char *)(v->u.object.values[i].name);
				n.name = strsep(&p, "@");
				n.addr = p ? strtoull(p, NULL, 0) : 0;
				n.value = (json_value *)(v->u.object.values[i].value);

				if(strcmp(drv->name, n.name) == 0)
					drv->probe(drv, &n);
			}
		}
		json_value_free(v);
	}
	return size;
}

/* 根据名称搜索驱动 */
struct driver_t * search_driver(const char * name)
{
	struct driver_t * pos;
	struct hlist_node * n;

	if(!name)
		return NULL;

	hlist_for_each_entry_safe(pos, n, driver_hash(name), node)
	{
		if(strcmp(pos->name, name) == 0)
			return pos;
	}
	return NULL;
}

/* 注册一个驱动 */
bool_t register_driver(struct driver_t * drv)
{
	irq_flags_t flags;

    /* 如果驱动空或无名驱动,则注册失败 */
	if(!drv || !drv->name)
		return FALSE;

	if(!drv->probe || !drv->remove)
		return FALSE;

	if(!drv->suspend || !drv->resume)
		return FALSE;

    /* 如果驱动名称已存在,则注册失败 */
	if(search_driver(drv->name))
		return FALSE;

    /* 根据驱动名称申请一个drivername路径kobj */
	drv->kobj = kobj_alloc_directory(drv->name);
    /* 在drivername路径kobj下添加一个probe文件kobj */
	kobj_add_regular(drv->kobj, "probe", NULL, driver_write_probe, drv);
    /* 在kobj/class/driver路径下挂载drivername路径 */
	kobj_add(search_class_driver_kobj(), drv->kobj);

	spin_lock_irqsave(&__driver_lock, flags);
    /* 驱动链表节点初始化 */
	init_hlist_node(&drv->node);
    /* 将驱动链表节点挂接到全局驱动哈希表中 */
	hlist_add_head(&drv->node, driver_hash(drv->name));
	spin_unlock_irqrestore(&__driver_lock, flags);

	return TRUE;
}

/* 注销驱动 */
bool_t unregister_driver(struct driver_t * drv)
{
	irq_flags_t flags;

	if(!drv || !drv->name)
		return FALSE;

	if(hlist_unhashed(&drv->node))
		return FALSE;

	spin_lock_irqsave(&__driver_lock, flags);
    /* 移除驱动链表节点 */
	hlist_del(&drv->node);
	spin_unlock_irqrestore(&__driver_lock, flags);
    /*  在kobj/class/driver下移除drivername路径kobj */
	kobj_remove_self(drv->kobj);

	return TRUE;
}

/* 探测设备 */
void probe_device(const char * json, int length)
{
	struct driver_t * drv;
	struct device_t * dev;
	struct dtnode_t n;
	json_value * v;
	char * p;
	int i;

	if(json && (length > 0))
	{
		v = json_parse(json, length);
		if(v && (v->type == json_object))
		{
			for(i = 0; i < v->u.object.length; i++)
			{
				p = (char *)(v->u.object.values[i].name);
				n.name = strsep(&p, "@");
				n.addr = p ? strtoull(p, NULL, 0) : 0;
				n.value = (json_value *)(v->u.object.values[i].value);

				drv = search_driver(n.name);
				if(drv && (dev = drv->probe(drv, &n)))
					LOG("Probe device '%s' with %s", dev->name, drv->name);
				else
					LOG("Fail to probe device with %s", n.name);
			}
		}
		json_value_free(v);
	}
}

/* 初始化全局驱动哈希表 */
static __init void driver_pure_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(__driver_hash); i++)
		init_hlist_head(&__driver_hash[i]);
}
pure_initcall(driver_pure_init);
