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

struct device_t
{
    struct kobj_t * kobj;
    struct list_head list;
    struct list_head head;
    struct hlist_node node;

    char * name;
    enum device_type_t type;
    struct driver_t * driver;
    void * priv;
};

extern struct list_head __device_list;
extern struct list_head __device_head[DEVICE_TYPE_MAX_COUNT];

char * alloc_device_name(const char * name, int id);
void free_device_name(char * name);
struct device_t * search_device(const char * name, enum device_type_t type);
struct device_t * search_first_device(enum device_type_t type);
bool_t register_device(struct device_t * dev);
bool_t unregister_device(struct device_t * dev);
bool_t register_device_notifier(struct notifier_t * n);
bool_t unregister_device_notifier(struct notifier_t * n);
void suspend_device(struct device_t * dev);
void resume_device(struct device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DEVICE_H__ */
