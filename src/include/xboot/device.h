#ifndef __DEVICE_H__
#define __DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <xboot/kobj.h>
#include <xboot/notifier.h>
#include <xboot/driver.h>

#if CONFIG_DEVICE_XDEF_EN
enum device_type_t {
#define Xdef(def, name)        DEVICE_TYPE_ ## def,
#include <xboot/device_table.h>
#undef Xdef
    DEVICE_TYPE_MAX_COUNT
};

#else
enum device_type_t {
    DEVICE_TYPE_ADC,
    DEVICE_TYPE_AUDIO,
    DEVICE_TYPE_BATTERY,
    DEVICE_TYPE_BLOCK,
    DEVICE_TYPE_BUZZER,
    DEVICE_TYPE_CAMERA,
    DEVICE_TYPE_CLK,
    DEVICE_TYPE_CLOCKEVENT,
    DEVICE_TYPE_CLOCKSOURCE,
    DEVICE_TYPE_COMPASS,
    DEVICE_TYPE_CONSOLE,
    DEVICE_TYPE_DAC,
    DEVICE_TYPE_DMACHIP,
    DEVICE_TYPE_FRAMEBUFFER,
    DEVICE_TYPE_GMETER,
	DEVICE_TYPE_GNSS,
    DEVICE_TYPE_GPIOCHIP,
    DEVICE_TYPE_GYROSCOPE,
    DEVICE_TYPE_HYGROMETER,
    DEVICE_TYPE_I2C,
    DEVICE_TYPE_INPUT,
    DEVICE_TYPE_IRQCHIP,
    DEVICE_TYPE_LED,
    DEVICE_TYPE_LEDSTRIP,
    DEVICE_TYPE_LEDTRIGGER,
    DEVICE_TYPE_LIGHT,
    DEVICE_TYPE_MOTOR,
    DEVICE_TYPE_NVMEM,
    DEVICE_TYPE_PRESSURE,
    DEVICE_TYPE_PROXIMITY,
    DEVICE_TYPE_PWM,
    DEVICE_TYPE_REGULATOR,
    DEVICE_TYPE_RESETCHIP,
    DEVICE_TYPE_RNG,
    DEVICE_TYPE_RTC,
    DEVICE_TYPE_SDHCI,
    DEVICE_TYPE_SERVO,
    DEVICE_TYPE_SPI,
    DEVICE_TYPE_STEPPER,
    DEVICE_TYPE_THERMOMETER,
    DEVICE_TYPE_UART,
    DEVICE_TYPE_VIBRATOR,
    DEVICE_TYPE_WATCHDOG,
    DEVICE_TYPE_MAX_COUNT
};
#endif

enum {
    NOTIFIER_DEVICE_ADD,                /* 设备添加 */
    NOTIFIER_DEVICE_REMOVE,             /* 设备移除 */
    NOTIFIER_DEVICE_SUSPEND,            /* 设备挂起 */
    NOTIFIER_DEVICE_RESUME,             /* 设备释放 */
};

struct driver_t;

/* 设备结构定义:设备为抽象类,需要绑定具体设备驱动 */
struct device_t
{
    struct kobj_t * kobj;
    struct list_head list;
    struct list_head head;
    struct hlist_node node;

    /* 设备名称 */
    char * name;
    /* 设备类型 */
    enum device_type_t type;
    /* 设备所使用的驱动:驱动为具体设备的驱动 */
    struct driver_t * driver;
    /* 设备私有数据,可用于挂接子类设备 */
    void * priv;
};

extern struct list_head __device_list;
extern struct list_head __device_head[DEVICE_TYPE_MAX_COUNT];

/* 根据设备主名和id号申请一个设备名称 */
char * alloc_device_name(const char * name, int id);
/* 释放一个设备名称 */
void free_device_name(char * name);

/* 提供给子类设备调用的API */
/* 查找一个名称和类型都匹配的设备 */
struct device_t * search_device(const char * name, enum device_type_t type);
/* 查找某类型的第一个设备 */
struct device_t * search_first_device(enum device_type_t type);
/* 注册一个设备 */
bool_t register_device(struct device_t * dev);
/* 注销一个设备 */
bool_t unregister_device(struct device_t * dev);

/* 注册一个设备通知 */
bool_t register_device_notifier(struct notifier_t * n);
/* 注销一个设备通知 */
bool_t unregister_device_notifier(struct notifier_t * n);
/* 通知设备已挂起 */
void suspend_device(struct device_t * dev);
/* 通知设备已释放 */
void resume_device(struct device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
