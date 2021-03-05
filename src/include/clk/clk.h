#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/* clk设备结构定义 */
struct clk_t
{
	char * name;
	int count;

	void (*set_parent)(struct clk_t * clk, const char * pname);
	const char * (*get_parent)(struct clk_t * clk);
	void (*set_enable)(struct clk_t * clk, bool_t enable);
	bool_t (*get_enable)(struct clk_t * clk);
	void (*set_rate)(struct clk_t * clk, u64_t prate, u64_t rate);
	u64_t (*get_rate)(struct clk_t * clk, u64_t prate);

	void * priv;
};

/* 根据名称搜索一个clk设备 */
struct clk_t * search_clk(const char * name);
/* 注册一个clk设备 */
struct device_t * register_clk(struct clk_t * clk, struct driver_t * drv);
/* 注销一个clk设备 */
void unregister_clk(struct clk_t * clk);

void clk_set_parent(const char * name, const char * pname);
const char * clk_get_parent(const char * name);
void clk_enable(const char * name);
void clk_disable(const char * name);
bool_t clk_status(const char * name);
void clk_set_rate(const char * name, u64_t rate);
u64_t clk_get_rate(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */

