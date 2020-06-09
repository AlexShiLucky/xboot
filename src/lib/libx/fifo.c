/*
 * libx/fifo.c
 */

#include <stddef.h>
#include <barrier.h>
#include <spinlock.h>
#include <log2.h>
#include <string.h>
#include <malloc.h>
#include <fifo.h>
#include <xboot/module.h>

/* FIFO复位 */
void __fifo_reset(struct fifo_t * f)
{
	f->in = f->out = 0;
}

/* FIFO中有效数据长度 */
unsigned int __fifo_len(struct fifo_t * f)
{
	return f->in - f->out;
}

/* 往FIFO中写入len长度的字符 */
unsigned int __fifo_put(struct fifo_t * f, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	len = min(len, f->size - f->in + f->out);
	smp_mb();
	l = min(len, f->size - (f->in & (f->size - 1)));
	memcpy(f->buffer + (f->in & (f->size - 1)), buf, l);
	memcpy(f->buffer, buf + l, len - l);
	smp_wmb();
	f->in += len;

	return len;
}

/* 从FIFO中获取len长度的字符 */
unsigned int __fifo_get(struct fifo_t * f, unsigned char * buf, unsigned int len)
{
	unsigned int l;

	len = min(len, f->in - f->out);
	smp_rmb();
	l = min(len, f->size - (f->out & (f->size - 1)));
	memcpy(buf, f->buffer + (f->out & (f->size - 1)), l);
	memcpy(buf + l, f->buffer, len - l);
	smp_mb();
	f->out += len;

	return len;
}

/* 分配一个size尺寸的FIFO */
struct fifo_t * fifo_alloc(unsigned int size)
{
	struct fifo_t * f;

	if(size & (size - 1))
		size = roundup_pow_of_two(size);

	f = malloc(sizeof(struct fifo_t));
	if(!f)
		return NULL;

    /* 申请一块size长度的空间 */
	f->buffer = malloc(size);
	if(!f->buffer)
	{
		free(f);
		return NULL;
	}
	f->size = size;
	f->in = 0;
	f->out = 0;
	spin_lock_init(&f->lock);

	return f;
}
EXPORT_SYMBOL(fifo_alloc);

/* 释放一个FIFO */
void fifo_free(struct fifo_t * f)
{
	if(f)
	{
		free(f->buffer);
		free(f);
	}
}
EXPORT_SYMBOL(fifo_free);

/* FIFO复位 */
void fifo_reset(struct fifo_t * f)
{
	irq_flags_t flags;

	spin_lock_irqsave(&f->lock, flags);
	__fifo_reset(f);
	spin_unlock_irqrestore(&f->lock, flags);
}
EXPORT_SYMBOL(fifo_reset);

/* 获取FIFO有效长度 */
unsigned int fifo_len(struct fifo_t * f)
{
	irq_flags_t flags;
	unsigned int ret;

	spin_lock_irqsave(&f->lock, flags);
	ret = __fifo_len(f);
	spin_unlock_irqrestore(&f->lock, flags);

	return ret;
}
EXPORT_SYMBOL(fifo_len);

/* 往FIFO中写入len长度的字符 */
unsigned int fifo_put(struct fifo_t * f, unsigned char * buf, unsigned int len)
{
	irq_flags_t flags;
	unsigned int ret;

	spin_lock_irqsave(&f->lock, flags);
	ret = __fifo_put(f, buf, len);
	spin_unlock_irqrestore(&f->lock, flags);

	return ret;
}

/* 从FIFO中获取len长度的字符 */
unsigned int fifo_get(struct fifo_t * f, unsigned char * buf, unsigned int len)
{
	irq_flags_t flags;
	unsigned int ret;

	spin_lock_irqsave(&f->lock, flags);
	ret = __fifo_get(f, buf, len);
	if(f->in == f->out)
		f->in = f->out = 0;
	spin_unlock_irqrestore(&f->lock, flags);

	return ret;
}
EXPORT_SYMBOL(fifo_get);
