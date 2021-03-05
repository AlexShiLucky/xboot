#ifndef __DAC_H__
#define __DAC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/* dac设备结构定义 */
struct dac_t
{
    /* dac设备公有数据 */
	char * name;
	int vreference;
	int resolution;
	int nchannel;

    /* dac设备接口 */
	void (*write)(struct dac_t * dac, int channel, u32_t value);
    /* dac设备私有数据 */
	void * priv;
};

/* 应用层API */
/* 根据名称搜索一个dac设备 */
struct dac_t * search_dac(const char * name);

/* 提供给驱动层调用的API */
/* 注册一个dac设备 */
struct device_t * register_dac(struct dac_t * dac, struct driver_t * drv);
/* 注销一个dac设备 */
void unregister_dac(struct dac_t * dac);

/* dac设备接口对应的API,内部就是调用设备接口函数 */
void dac_write_raw(struct dac_t * dac, int channel, u32_t value);
void dac_write_voltage(struct dac_t * dac, int channel, int voltage);

#ifdef __cplusplus
}
#endif

#endif /* __DAC_H__ */
