#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xconfigs.h>
#include <types.h>
#include <stdint.h>
#include <list.h>
#include <irqflags.h>
#include <spinlock.h>
#include <smp.h>
#include <rbtree_augmented.h>

struct task_t;
struct scheduler_t;
typedef void (*task_func_t)(struct task_t * task, void * data);

enum task_status_t {
	TASK_STATUS_RUNNING	= 0,    // 任务运行状态
	TASK_STATUS_READY	= 1,    // 任务就绪状态
	TASK_STATUS_SUSPEND	= 2,    // 任务挂起状态
};

/* 任务结构 */
struct task_t {
	struct rb_node node;
	struct list_head list;
	struct list_head slist;
	struct list_head rlist;
	struct list_head mlist;
    /* 任务调度器*/
	struct scheduler_t * sched;
    /* 任务状态 */
	enum task_status_t status;
	uint64_t start;
	uint64_t time;
	uint64_t vtime;
    /* 任务名称 */
	char * name;
    /* 任务上下文 */
	void * fctx;
    /* 任务堆栈 */
	void * stack;
    /* 任务栈深度 */
	size_t stksz;
    /* 任务优先级 */
	int nice;
    /* 任务负荷权重 */
	int weight;
    /* 权重值用于重除的结果 */
	uint32_t inv_weight;
    /* 任务入口函数 */
	task_func_t func;
    /* 任务入口函数参数 */
	void * data;
    /* 任务错误码 */
	int __errno;
};

/* 调度器结构 */
struct scheduler_t {
	struct rb_root_cached ready;
	struct list_head suspend;
	struct task_t * running;
	uint64_t min_vtime;
    /* 调度器的负荷权重 */
	uint64_t weight;
	spinlock_t lock;
};

extern struct scheduler_t __sched[CONFIG_MAX_SMP_CPUS];

/* 获取当前处理器对应的调度器 */
static inline struct scheduler_t * scheduler_self(void)
{
	return &__sched[smp_processor_id()];
}

/* 获取当前调度器上运行的任务 */
static inline struct task_t * task_self(void)
{
	return __sched[smp_processor_id()].running;
}

/* 任务创建 */
struct task_t * task_create(struct scheduler_t * sched, const char * name, task_func_t func, void * data, size_t stksz, int nice);
/* 任务销毁 */
void task_destroy(struct task_t * task);
/* 重新设置任务优先级 */
void task_renice(struct task_t * task, int nice);
/* 任务挂起 */
void task_suspend(struct task_t * task);
/* 任务恢复 */
void task_resume(struct task_t * task);
/* 任务让出 */
void task_yield(void);

/* 调度器循环调度 */
void scheduler_loop(void);
/* 初始化调度器 */
void do_init_sched(void);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_H__ */
