/*
 * driver/clk/clk-mux.c
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
#include <clk/clk.h>

struct clk_mux_parent_t {
	char * name;
	int value;
};

/* mux-clk设备私有结构 */
struct clk_mux_pdata_t {
	virtual_addr_t virt;
	struct clk_mux_parent_t * parent;
	int nparent;
	int shift;
	int width;
};

/* mux-clk设备父clk设置接口具体实现 */
static void clk_mux_set_parent(struct clk_t * clk, const char * pname)
{
	struct clk_mux_pdata_t * pdat = (struct clk_mux_pdata_t *)clk->priv;
	u32_t val;
	int i;

	for(i = 0; i < pdat->nparent; i++)
	{
		if(strcmp(pdat->parent[i].name, pname) == 0)
		{
			val = read32(pdat->virt);
			val &= ~(((1 << pdat->width) - 1) << pdat->shift);
			val |= pdat->parent[i].value << pdat->shift;
			write32(pdat->virt, val);
			break;
		}
	}
}

/* mux-clk设备父clk获取具体实现 */
static const char * clk_mux_get_parent(struct clk_t * clk)
{
	struct clk_mux_pdata_t * pdat = (struct clk_mux_pdata_t *)clk->priv;
	int val = (read32(pdat->virt) >> pdat->shift) & ((1 << pdat->width) - 1);
	int i;

	for(i = 0; i < pdat->nparent; i++)
	{
		if(pdat->parent[i].value == val)
			return pdat->parent[i].name;
	}
	return NULL;
}

/* mux-clk设备使能设置具体实现 */
static void clk_mux_set_enable(struct clk_t * clk, bool_t enable)
{
}

/* mux-clk设备使能获取具体实现 */
static bool_t clk_mux_get_enable(struct clk_t * clk)
{
	return TRUE;
}

/* mux-clk设备速率设置具体实现 */
static void clk_mux_set_rate(struct clk_t * clk, u64_t prate, u64_t rate)
{
}

/* mux-clk设备速率获取具体实现 */
static u64_t clk_mux_get_rate(struct clk_t * clk, u64_t prate)
{
	return prate;
}

/* mux-clk设备探针 */
static struct device_t * clk_mux_probe(struct driver_t * drv, struct dtnode_t * n)
{
	struct clk_mux_pdata_t * pdat;
	struct clk_mux_parent_t * parent;
	struct clk_t * clk;
	struct device_t * dev;
	struct dtnode_t o;
	virtual_addr_t virt = phys_to_virt(dt_read_address(n));
	char * name = dt_read_string(n, "name", NULL);
	int nparent = dt_read_array_length(n, "parent");
	int shift = dt_read_int(n, "shift", -1);
	int width = dt_read_int(n, "width", -1);
	int i;

	if(!name || (nparent <= 0) || (shift < 0) || (width <= 0))
		return NULL;

	if(search_clk(name))
		return NULL;

	pdat = malloc(sizeof(struct clk_mux_pdata_t));
	if(!pdat)
		return NULL;

	parent = malloc(sizeof(struct clk_mux_parent_t) * nparent);
	if(!parent)
	{
		free(pdat);
		return NULL;
	}

	clk = malloc(sizeof(struct clk_t));
	if(!clk)
	{
		free(pdat);
		free(parent);
		return NULL;
	}

	for(i = 0; i < nparent; i++)
	{
		dt_read_array_object(n, "parent", i, &o);
		parent[i].name = strdup(dt_read_string(&o, "name", NULL));
		parent[i].value = dt_read_int(&o, "value", 0);
	}

	pdat->virt = virt;
	pdat->parent = parent;
	pdat->nparent = nparent;
	pdat->shift = shift;
	pdat->width = width;

	clk->name = strdup(name);
	clk->count = 0;
	clk->set_parent = clk_mux_set_parent;
	clk->get_parent = clk_mux_get_parent;
	clk->set_enable = clk_mux_set_enable;
	clk->get_enable = clk_mux_get_enable;
	clk->set_rate = clk_mux_set_rate;
	clk->get_rate = clk_mux_get_rate;
	clk->priv = pdat;

	if(!register_clk(&dev, clk))
	{
		for(i = 0; i < pdat->nparent; i++)
			free(pdat->parent[i].name);
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
		return NULL;
	}
	dev->driver = drv;

	if(dt_read_object(n, "default", &o))
	{
		char * c = clk->name;
		char * p;
		u64_t r;
		int e;

		if((p = dt_read_string(&o, "parent", NULL)) && search_clk(p))
			clk_set_parent(c, p);
		if((r = (u64_t)dt_read_long(&o, "rate", 0)) > 0)
			clk_set_rate(c, r);
		if((e = dt_read_bool(&o, "enable", -1)) != -1)
		{
			if(e > 0)
				clk_enable(c);
			else
				clk_disable(c);
		}
	}
	return dev;
}

/* mux-clk设备移除 */
static void clk_mux_remove(struct device_t * dev)
{
	struct clk_t * clk = (struct clk_t *)dev->priv;
	struct clk_mux_pdata_t * pdat = (struct clk_mux_pdata_t *)clk->priv;
	int i;

	if(clk && unregister_clk(clk))
	{
		for(i = 0; i < pdat->nparent; i++)
			free(pdat->parent[i].name);
		free(pdat->parent);

		free(clk->name);
		free(clk->priv);
		free(clk);
	}
}

/* mux-clk设备挂起 */
static void clk_mux_suspend(struct device_t * dev)
{
}

/* mux-clk设备释放 */
static void clk_mux_resume(struct device_t * dev)
{
}

/* mux-clk设备驱动控制块 */
static struct driver_t clk_mux = {
	.name		= "clk-mux",
	.probe		= clk_mux_probe,
	.remove		= clk_mux_remove,
	.suspend	= clk_mux_suspend,
	.resume		= clk_mux_resume,
};

/* mux-clk设备驱动初始化 */
static __init void clk_mux_driver_init(void)
{
	register_driver(&clk_mux);
}

/* mux-clk设备驱动退出 */
static __exit void clk_mux_driver_exit(void)
{
	unregister_driver(&clk_mux);
}

driver_initcall(clk_mux_driver_init);
driver_exitcall(clk_mux_driver_exit);
