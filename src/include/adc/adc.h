#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

/* adc设备结构定义 */
struct adc_t
{
    /* adc设备公有数据 */
	char * name;
	int vreference;
	int resolution;
	int nchannel;

    /* adc设备接口 */
	u32_t (*read)(struct adc_t * adc, int channel);
    /* adc设备私有数据 */
	void * priv;
};

/* 应用层API */
/* 根据名称搜索一个adc设备 */
struct adc_t * search_adc(const char * name);

/* 提供给驱动层调用的API */
/* 注册一个adc设备 */
struct device_t * register_adc(struct adc_t * adc, struct driver_t * drv);
void unregister_adc(struct adc_t * adc);

/* adc设备接口对应的API,内部就是调用设备接口函数 */
u32_t adc_read_raw(struct adc_t * adc, int channel);
int adc_read_voltage(struct adc_t * adc, int channel);

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */
