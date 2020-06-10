/*
 * kernel/core/task.c
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
#include <xboot/task.h>

struct transfer_t
{
	void * fctx;
	void * priv;
};

extern void * make_fcontext(void * stack, size_t size, void (*func)(struct transfer_t));
extern struct transfer_t jump_fcontext(void * fctx, void * priv);

/* 调度器表 */
struct scheduler_t __sched[CONFIG_MAX_SMP_CPUS];
EXPORT_SYMBOL(__sched);

/*
 * 任务每降低一个nice值(优先级提升),则多获得10%的CPU时间;
 * 任务每升高一个nice值(优先级降低),则放弃10%的CPU时间.
 */
static const int nice_to_weight[40] = {
 /* -20 */     88761,     71755,     56483,     46273,     36291,
 /* -15 */     29154,     23254,     18705,     14949,     11916,
 /* -10 */      9548,      7620,      6100,      4904,      3906,
 /*  -5 */      3121,      2501,      1991,      1586,      1277,
 /*   0 */      1024,       820,       655,       526,       423,
 /*   5 */       335,       272,       215,       172,       137,
 /*  10 */       110,        87,        70,        56,        45,
 /*  15 */        36,        29,        23,        18,        15,
};

static const uint32_t nice_to_wmult[40] = {
 /* -20 */     48388,     59856,     76040,     92818,    118348,
 /* -15 */    147320,    184698,    229616,    287308,    360437,
 /* -10 */    449829,    563644,    704093,    875809,   1099582,
 /*  -5 */   1376151,   1717300,   2157191,   2708050,   3363326,
 /*   0 */   4194304,   5237765,   6557202,   8165337,  10153587,
 /*   5 */  12820798,  15790321,  19976592,  24970740,  31350126,
 /*  10 */  39045157,  49367440,  61356676,  76695844,  95443717,
 /*  15 */ 119304647, 148102320, 186737708, 238609294, 286331153,
};

/* 返回两个32位数相乘结果 */
static inline uint64_t mul_u32_u32(uint32_t a, uint32_t b)
{
	return (uint64_t)a * b;
}

static inline uint64_t mul_u64_u32_shr(uint64_t a, uint32_t mul, unsigned int shift)
{
	uint32_t ah, al;
	uint64_t ret;

	al = a;
	ah = a >> 32;

	ret = mul_u32_u32(al, mul) >> shift;
	if(ah)
		ret += mul_u32_u32(ah, mul) << (32 - shift);
	return ret;
}

static inline uint64_t calc_delta(struct task_t * task, uint64_t delta)
{
	uint64_t fact = 1024;
	int shift = 32;

	if(unlikely(fact >> 32))
	{
		while(fact >> 32)
		{
			fact >>= 1;
			shift--;
		}
	}
	fact = (uint64_t)(uint32_t)fact * task->inv_weight;

	while(fact >> 32)
	{
		fact >>= 1;
		shift--;
	}
	return mul_u64_u32_shr(delta, fact, shift);
}

static inline uint64_t calc_delta_fair(struct task_t * task, uint64_t delta)
{
	if(unlikely(task->weight != 1024))
		delta = calc_delta(task, delta);
	return delta;
}

/* 获取调度器中下一个就绪任务 */
static inline struct task_t * scheduler_next_ready_task(struct scheduler_t * sched)
{
	struct rb_node * leftmost = rb_first_cached(&sched->ready);

	if(!leftmost)
		return NULL;
	return rb_entry(leftmost, struct task_t, node);
}

/* 调度器入队一个任务 */
static inline void scheduler_enqueue_task(struct scheduler_t * sched, struct task_t * task)
{
	struct rb_node ** link = &sched->ready.rb_root.rb_node;
	struct rb_node * parent = NULL;
	struct task_t * next, * entry;
	int leftmost = 1;

	while(*link)
	{
		parent = *link;
		entry = rb_entry(parent, struct task_t, node);
		if((int64_t)(task->vtime - entry->vtime) < 0)
		{
			link = &parent->rb_left;
		}
		else
		{
			link = &parent->rb_right;
			leftmost = 0;
		}
	}

	rb_link_node(&task->node, parent, link);
	rb_insert_color_cached(&task->node, &sched->ready, leftmost);
	next = scheduler_next_ready_task(sched);
	if(likely(next))
		sched->min_vtime = next->vtime;
	else if(sched->running)
		sched->min_vtime = sched->running->vtime;
	else
		sched->min_vtime = 0;
}

/* 调度器出队一个任务 */
static inline void scheduler_dequeue_task(struct scheduler_t * sched, struct task_t * task)
{
	struct task_t * next;

	rb_erase_cached(&task->node, &sched->ready);
	next = scheduler_next_ready_task(sched);
	if(likely(next))
		sched->min_vtime = next->vtime;
	else if(sched->running)
		sched->min_vtime = sched->running->vtime;
	else
		sched->min_vtime = 0;
}

/* 调度器任务切换 */
static inline void scheduler_switch_task(struct scheduler_t * sched, struct task_t * task)
{
	struct task_t * running = sched->running;
	sched->running = task;
	struct transfer_t from = jump_fcontext(task->fctx, running);
	struct task_t * t = (struct task_t *)from.priv;
	t->fctx = from.fctx;
}

/* 调度器均衡选择,返回最轻的调度器 */
static inline struct scheduler_t * scheduler_load_balance_choice(void)
{
	struct scheduler_t * sched;
	uint64_t weight = ~0ULL;
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
	{
	    /* 根据调度器的weight属性,逐一查找weight最小的调度器 */
		if(__sched[i].weight < weight)
		{
			sched = &__sched[i];
			weight = __sched[i].weight;
		}
	}
	return sched;
}

static void fcontext_entry_func(struct transfer_t from)
{
	struct task_t * t = (struct task_t *)from.priv;
	struct scheduler_t * sched = t->sched;
	struct task_t * next, * task = sched->running;

	t->fctx = from.fctx;
	task->func(task, task->data);
	task_destroy(task);

	next = scheduler_next_ready_task(sched);
	if(likely(next))
	{
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

/* 任务创建 */
struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t stksz, int nice)
{
	struct task_t * task;
	void * stack;

	if(!func)
		return NULL;

    /* 如果未指定调度器,则自动分配最轻的调度器 */
	if(!sched)
		sched = scheduler_load_balance_choice();

    /* 如果未设置栈深度,则使用默认设置 */
	if(stksz <= 0)
		stksz = CONFIG_TASK_STACK_SIZE;

    /* 任务优先级 */
	if(nice < -20)
		nice = -20;
	else if(nice > 19)
		nice = 19;

    /* 分配一个任务控制块 */
	task = malloc(sizeof(struct task_t));
	if(!task)
		return NULL;

    /* 分配栈空间 */
	stack = malloc(stksz);
	if(!stack)
	{
		free(task);
		return NULL;
	}

    /* 任务控制块内成员初始化 */
	RB_CLEAR_NODE(&task->node);
	init_list_head(&task->list);
	init_list_head(&task->slist);
	init_list_head(&task->rlist);
	init_list_head(&task->mlist);
    /* 调度器上锁 */
	spin_lock(&sched->lock);
    /* 将创建的任务连接到挂起链表 */
	list_add_tail(&task->list, &sched->suspend);
    /* 添加任务后.增加调度器负荷权重 */
	sched->weight += nice_to_weight[nice + 20];
    /* 调度器解锁 */
	spin_unlock(&sched->lock);

    /* 设置任务名称 */
	task->name = strdup(name);
    /* 新建的任务初始状态为Suspend状态 */
	task->status = TASK_STATUS_SUSPEND;
    /* 任务创建时刻 */
	task->start = ktime_to_ns(ktime_get());
	task->time = 0;
	task->vtime = 0;
    /* 设置任务所在调度器 */
	task->sched = sched;
    /* 设置任务栈 */
	task->stack = stack;
    /* 设置任务栈深度 */
	task->stksz = stksz;
    /* 设置任务优先级 */
	task->nice = nice;
    /* 设置任务负荷权重值 */
	task->weight = nice_to_weight[nice + 20];
	task->inv_weight = nice_to_wmult[nice + 20];
    /* 创建任务上下文 */
	task->fctx = make_fcontext(task->stack + stksz, task->stksz, fcontext_entry_func);
    /* 设置任务入口函数 */
	task->func = func;
    /* 设置任务入口函数参数 */
	task->data = data;
    /* 设置任务的错误号 */
	task->__errno = 0;

	return task;
}

/* 任务销毁 */
void task_destroy(struct task_t * task)
{
	if(task)
	{
	    /* 调度器上锁 */
		spin_lock(&task->sched->lock);
        /* 销毁任务后.减小调度器负荷权重 */
		task->sched->weight -= nice_to_weight[task->nice + 20];
        /* 调度器解锁 */
		spin_unlock(&task->sched->lock);

        /* 释放任务名称 */
		if(task->name)
			free(task->name);
        /* 释放任务栈 */
		free(task->stack);
        /* 释放任务控制块 */
		free(task);
	}
}

/* 重新设置任务优先级 */
void task_renice(struct task_t * task, int nice)
{
	if(nice < -20)
		nice = -20;
	else if(nice > 19)
		nice = 19;

	if(task->nice != nice)
	{
	    /* 调度器上锁 */
		spin_lock(&task->sched->lock);
        /* 修改任务优先级后,先减小原先调度器负荷权重 */
		task->sched->weight -= nice_to_weight[task->nice + 20];
        /* 后再加上现在调度器负荷权重 */
		task->sched->weight += nice_to_weight[nice + 20];
        /* 调度器解锁 */
		spin_unlock(&task->sched->lock);

        /* 修改任务优先级 */
		task->nice = nice;
        /* 修改任务负荷权重 */
		task->weight = nice_to_weight[nice + 20];
		task->inv_weight = nice_to_wmult[nice + 20];
	}
}

/* 任务挂起 */
void task_suspend(struct task_t * task)
{
	struct task_t * next;
	uint64_t now, detla;

	if(task)
	{
	    /* 任务处于就绪状态 */
		if(task->status == TASK_STATUS_READY)
		{
			task->status = TASK_STATUS_SUSPEND;
            /* 调度器上锁 */
			spin_lock(&task->sched->lock);
            /* 将任务连接到挂起链表 */
			list_add_tail(&task->list, &task->sched->suspend);
            /* 调度器解锁 */
			spin_unlock(&task->sched->lock);
            /* 任务从调度器中出队 */
			scheduler_dequeue_task(task->sched, task);
		}
        /* 任务处于运行状态 */
		else if(task->status == TASK_STATUS_RUNNING)
		{
		    /* 获取当前时刻 */
			now = ktime_to_ns(ktime_get());
            /* 计算任务运行耗时 */
			detla = now - task->start;

			task->time += detla;
			task->vtime += calc_delta_fair(task, detla);
			task->status = TASK_STATUS_SUSPEND;
            /* 调度器上锁 */
			spin_lock(&task->sched->lock);
            /* 将任务连接到挂起链表 */
			list_add_tail(&task->list, &task->sched->suspend);
            /* 调度器解锁 */
			spin_unlock(&task->sched->lock);

            /* 获取下一个就绪任务 */
			next = scheduler_next_ready_task(task->sched);
			if(next)
			{
			    /* 任务从调度器中出队 */
				scheduler_dequeue_task(task->sched, next);
				next->status = TASK_STATUS_RUNNING;
				next->start = now;
                /* 任务切换 */
				scheduler_switch_task(task->sched, next);
			}
		}
	}
}

/* 任务释放 */
void task_resume(struct task_t * task)
{
	if(task && (task->status == TASK_STATUS_SUSPEND))
	{
		task->vtime = task->sched->min_vtime;
		task->status = TASK_STATUS_READY;
		spin_lock(&task->sched->lock);
		list_del_init(&task->list);
		spin_unlock(&task->sched->lock);
        /* 调度器入队该任务 */
		scheduler_enqueue_task(task->sched, task);
	}
}

/* 任务让出 */
void task_yield(void)
{
	struct scheduler_t * sched = scheduler_self();
	struct task_t * next, * self = task_self();
	uint64_t now = ktime_to_ns(ktime_get());
	uint64_t detla = now - self->start;

	self->time += detla;
	self->vtime += calc_delta_fair(self, detla);

	if((int64_t)(self->vtime - sched->min_vtime) < 0)
	{
		self->start = now;
	}
	else
	{
		self->status = TASK_STATUS_READY;
        /* 调度器入队该任务 */
		scheduler_enqueue_task(sched, self);
        /* 获取下一个就绪任务 */
		next = scheduler_next_ready_task(sched);
        /* 调度器出队下一个任务 */
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = now;
        /* 调度器切换到下一个任务 */
		if(likely(next != self))
			scheduler_switch_task(sched, next);
	}
}

/* 空闲任务 */
static void idle_task(struct task_t * task, void * data)
{
	while(1)
	{
		task_yield();
	}
}

static void smpboot_entry_func(void)
{
	machine_smpinit();

	struct scheduler_t * sched = scheduler_self();
	struct task_t * task = task_create(sched, "idle", idle_task, (void *)(unsigned long)(smp_processor_id()), SZ_8K, 0);
	spin_lock(&sched->lock);
	sched->weight -= task->weight;
	task->nice = 26;
	task->weight = 3;
	task->inv_weight = 1431655765;
	sched->weight += task->weight;
	spin_unlock(&sched->lock);
	task_resume(task);

	struct task_t * next = scheduler_next_ready_task(sched);
	if(next)
	{
		sched->running = next;
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

/* 调度器循环调度 */
void scheduler_loop(void)
{
	machine_smpboot(smpboot_entry_func);

	struct scheduler_t * sched = scheduler_self();
	struct task_t * task = task_create(sched, "idle", idle_task, (void *)(unsigned long)smp_processor_id(), SZ_8K, 0);
	spin_lock(&sched->lock);
	sched->weight -= task->weight;
	task->nice = 26;
	task->weight = 3;
	task->inv_weight = 1431655765;
	sched->weight += task->weight;
	spin_unlock(&sched->lock);
	task_resume(task);

	struct task_t * next = scheduler_next_ready_task(sched);
	if(next)
	{
		sched->running = next;
		scheduler_dequeue_task(sched, next);
		next->status = TASK_STATUS_RUNNING;
		next->start = ktime_to_ns(ktime_get());
		scheduler_switch_task(sched, next);
	}
}

/* 初始化调度器 */
void do_init_sched(void)
{
	struct scheduler_t * sched;
	int i;

	for(i = 0; i < CONFIG_MAX_SMP_CPUS; i++)
	{
		sched = &__sched[i];

		spin_lock_init(&sched->lock);
		spin_lock(&sched->lock);
		sched->ready = RB_ROOT_CACHED;
		init_list_head(&sched->suspend);
		sched->running = NULL;
		sched->min_vtime = 0;
		sched->weight = 0;
		spin_unlock(&sched->lock);
	}
}
