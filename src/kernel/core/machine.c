/*
 * kernel/core/machine.c
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
#include <sha256.h>
#include <watchdog/watchdog.h>
#include <xboot/machine.h>

/* 全局机器链表 */
static struct list_head __machine_list = {
    .next = &__machine_list,
    .prev = &__machine_list,
};
static spinlock_t __machine_lock = SPIN_LOCK_INIT();
/* 当前机器指针 */
static struct machine_t * __machine = NULL;

static const char * __machine_uniqueid(struct machine_t * mach)
{
    const char * id = NULL;

    if(mach && mach->uniqueid)
        id = mach->uniqueid(mach);
    return id ? id : "0123456789";
}

/* 搜索机器kobj */
static struct kobj_t * search_class_machine_kobj(void)
{
    /* 在kobj下搜索(创建)class */
    struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
    /* 在kobj/class下创建machine路径 */
    return kobj_search_directory_with_create(kclass, "machine");
}

static ssize_t machine_read_description(struct kobj_t * kobj, void * buf, size_t size)
{
    struct machine_t * mach = (struct machine_t *)kobj->priv;
    return sprintf(buf, "%s", mach->desc);
}

static ssize_t machine_read_mmap(struct kobj_t * kobj, void * buf, size_t size)
{
    struct machine_t * mach = (struct machine_t *)kobj->priv;
    struct mmap_t * pos, * n;
    char * p = buf;
    int len = 0;

    list_for_each_entry_safe(pos, n, &mach->mmap, list)
    {
        len += sprintf((char *)(p + len), " %s: %p:%p - %p:%p\r\n", pos->name, pos->virt, pos->size, pos->phys, pos->size);
    }
    return len;
}

static ssize_t machine_read_uniqueid(struct kobj_t * kobj, void * buf, size_t size)
{
    struct machine_t * mach = (struct machine_t *)kobj->priv;
    return sprintf(buf, "%s", __machine_uniqueid(mach));
}

/* 根据名称搜索机器 */
static struct machine_t * search_machine(const char * name)
{
    struct machine_t * pos, * n;

    if(!name)
        return NULL;

    list_for_each_entry_safe(pos, n, &__machine_list, list)
    {
        if(strcmp(pos->name, name) == 0)
            return pos;
    }
    return NULL;
}

/* 注册机器 */
bool_t register_machine(struct machine_t * mach)
{
    irq_flags_t flags;
    int i;

    if(!mach || !mach->name || !mach->detect)
        return FALSE;

    /* 如果机器名称已存在,则注册失败 */
    if(search_machine(mach->name))
        return FALSE;

    /* 根据机器名称申请一个machname路径kobj */
    mach->kobj = kobj_alloc_directory(mach->name);
    /* 在machname路径kobj下添加一个description文件kobj */
    kobj_add_regular(mach->kobj, "description", machine_read_description, NULL, mach);
    /* 在machname路径kobj下添加一个mmap文件kobj */
    kobj_add_regular(mach->kobj, "mmap", machine_read_mmap, NULL, mach);
    /* 在machname路径kobj下添加一个uniqueid文件kobj */
    kobj_add_regular(mach->kobj, "uniqueid", machine_read_uniqueid, NULL, mach);
    /* 在kobj/class/machine路径下挂载machname路径 */
    kobj_add(search_class_machine_kobj(), mach->kobj);

    spin_lock_irqsave(&__machine_lock, flags);
    /* 机器链表节点初始化 */
    init_list_head(&mach->list);
    /* 内存映射链表初始化 */
    init_list_head(&mach->mmap);
    /* 将机器链表节点挂接到全局机器链表中 */
    list_add_tail(&mach->list, &__machine_list);
    spin_unlock_irqrestore(&__machine_lock, flags);

    if(!__machine && (mach->detect(mach) > 0))
    {
        __machine = mach;
        if(mach->memmap)
        {
            mach->memmap(mach);
        }
        if(mach->logger)
        {
            for(i = 0; i < 5; i++)
            {
                mach->logger(mach, xboot_character_logo_string(i), strlen(xboot_character_logo_string(i)));
                mach->logger(mach, "\r\n", 2);
            }
            mach->logger(mach, xboot_banner_string(), strlen(xboot_banner_string()));
            mach->logger(mach, " - [", 4);
            mach->logger(mach, mach->name, strlen(mach->name));
            mach->logger(mach, "][", 2);
            mach->logger(mach, mach->desc, strlen(mach->desc));
            mach->logger(mach, "]\r\n", 3);
        }
    }
    return TRUE;
}

/* 注销机器 */
bool_t unregister_machine(struct machine_t * mach)
{
    struct mmap_t * pos, * n;
    irq_flags_t flags;

    if(!mach || !mach->name)
        return FALSE;

    spin_lock_irqsave(&__machine_lock, flags);
    list_for_each_entry_safe(pos, n, &mach->mmap, list)
    {
        list_del(&pos->list);
    }
    spin_unlock_irqrestore(&__machine_lock, flags);

    spin_lock_irqsave(&__machine_lock, flags);
    /* 移除机器链表节点 */
    list_del(&mach->list);
    spin_unlock_irqrestore(&__machine_lock, flags);
    /* 在kobj/class/machine下移除machname */
    kobj_remove(search_class_machine_kobj(), mach->kobj);
    /* 移除machname路径下所有文件kobj和自身 */
    kobj_remove_self(mach->kobj);

    return TRUE;
}

bool_t machine_mmap(struct machine_t * mach, const char * name, virtual_addr_t virt, physical_addr_t phys, physical_size_t size, int type)
{
    struct mmap_t * m;
    irq_flags_t flags;

    if(!mach || !name || (size == 0))
        return FALSE;

    m = malloc(sizeof(struct mmap_t));
    if(!m)
        return FALSE;

    m->name = name;
    m->virt = virt;
    m->phys = phys;
    m->size = size;
    m->type = type;

    spin_lock_irqsave(&__machine_lock, flags);
    init_list_head(&m->list);
    list_add_tail(&m->list, &mach->mmap);
    spin_unlock_irqrestore(&__machine_lock, flags);

    return TRUE;
}

/* 获取当前机器 */
inline __attribute__((always_inline)) struct machine_t * get_machine(void)
{
    return __machine;
}

/* 机器关机 */
void machine_shutdown(void)
{
    struct machine_t * mach = get_machine();

    sync();
    if(mach && mach->shutdown)
        mach->shutdown(mach);
}

/* 机器重启 */
void machine_reboot(void)
{
    struct machine_t * mach = get_machine();

    sync();
    if(mach && mach->reboot)
        mach->reboot(mach);
    watchdog_set_timeout(search_first_watchdog(), 1);
}

/* 机器睡眠 */
void machine_sleep(void)
{
    struct machine_t * mach = get_machine();
    struct device_t * pos, * n;

    sync();
    list_for_each_entry_safe_reverse(pos, n, &__device_list, list)
    {
        suspend_device(pos);
    }
    if(mach && mach->sleep)
    {
        mach->sleep(mach);
    }
    list_for_each_entry_safe(pos, n, &__device_list, list)
    {
        resume_device(pos);
    }
}

/* 机器清理 */
void machine_cleanup(void)
{
    struct machine_t * mach = get_machine();

    sync();
    if(mach && mach->cleanup)
        mach->cleanup(mach);
}

/* 机器log输出 */
int machine_logger(const char * fmt, ...)
{
    struct machine_t * mach = get_machine();
    struct timeval tv;
    char buf[SZ_4K];
    int len = 0;
    va_list ap;

    if(mach && mach->logger)
    {
        va_start(ap, fmt);
        gettimeofday(&tv, 0);
        len += sprintf((char *)(buf + len), "[%5u.%06u]", tv.tv_sec, tv.tv_usec);
        len += vsnprintf((char *)(buf + len), (SZ_4K - len), fmt, ap);
        va_end(ap);
        mach->logger(mach, (const char *)buf, len);
    }
    return len;
}

/* 获取机器唯一标识符 */
const char * machine_uniqueid(void)
{
    struct machine_t * mach = get_machine();
    return __machine_uniqueid(mach);
}

/* 机器keygen */
int machine_keygen(const char * msg, void * key)
{
    struct machine_t * mach = get_machine();
    int len;

    if(mach && mach->keygen && ((len = mach->keygen(mach, msg, key)) > 0))
        return len;
    sha256_hash(msg, strlen(msg), key);
    return 32;
}

/* 机器物理地址转虚拟地址 */
static virtual_addr_t __phys_to_virt(physical_addr_t phys)
{
    struct machine_t * mach = get_machine();
    struct mmap_t * pos;

    if(mach)
    {
        list_for_each_entry(pos, &mach->mmap, list)
        {
            if((phys >= pos->phys) && (phys < pos->phys + pos->size))
                return (virtual_addr_t)(pos->virt + (phys - pos->phys));
        }
    }
    return (virtual_addr_t)phys;
}
extern __typeof(__phys_to_virt) phys_to_virt __attribute__((weak, alias("__phys_to_virt")));

/* 机器虚拟地址转物理地址 */
static physical_addr_t __virt_to_phys(virtual_addr_t virt)
{
    struct machine_t * mach = get_machine();
    struct mmap_t * pos;

    if(mach)
    {
        list_for_each_entry(pos, &mach->mmap, list)
        {
            if((virt >= pos->virt) && (virt < pos->virt + pos->size))
                return (physical_addr_t)(pos->phys + (virt - pos->virt));
        }
    }
    return (physical_addr_t)virt;
}
extern __typeof(__virt_to_phys) virt_to_phys __attribute__((weak, alias("__virt_to_phys")));
