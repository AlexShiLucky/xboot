#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum irq_type_t {
	IRQ_TYPE_NONE			= 0,    /* 无效IRQ类型 */
	IRQ_TYPE_LEVEL_LOW		= 1,    /* 低电平IRQ类型 */
	IRQ_TYPE_LEVEL_HIGH		= 2,    /* 高电平IRQ类型 */
	IRQ_TYPE_EDGE_FALLING	= 3,    /* 下降沿IRQ类型 */
	IRQ_TYPE_EDGE_RISING	= 4,    /* 上升沿IRQ类型 */
	IRQ_TYPE_EDGE_BOTH		= 5,    /* 双边沿IRQ类型 */
};

struct irq_handler_t {
	void (*func)(void * data);
	void * data;
};

struct irqchip_t
{
	char * name;
	int base;
	int nirq;

	struct irq_handler_t * handler;
	void (*enable)(struct irqchip_t * chip, int offset);
	void (*disable)(struct irqchip_t * chip, int offset);
	void (*settype)(struct irqchip_t * chip, int offset, enum irq_type_t type);
	void (*dispatch)(struct irqchip_t * chip);

	void * priv;
};

struct device_t * register_irqchip(struct irqchip_t * chip, struct driver_t * drv);
void unregister_irqchip(struct irqchip_t * chip);
struct device_t * register_sub_irqchip(int parent, struct irqchip_t * chip, struct driver_t * drv);
void unregister_sub_irqchip(int parent, struct irqchip_t * chip);
bool_t irq_is_valid(int irq);
bool_t request_irq(int irq, void (*func)(void *), enum irq_type_t type, void * data);
bool_t free_irq(int irq);
void enable_irq(int irq);
void disable_irq(int irq);
void interrupt_handle_exception(void * regs);

#ifdef __cplusplus
}
#endif

#endif /* __INTERRUPT_H__ */
