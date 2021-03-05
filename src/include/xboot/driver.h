#ifndef __DRIVER_H__
#define __DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <xboot/kobj.h>
#include <xboot/dtree.h>
#include <xboot/device.h>

struct device_t;

/* 驱动结构定义 */
struct driver_t
{
    struct kobj_t * kobj;
    struct hlist_node node;

    /* 驱动名称 */
    char * name;

    /* 驱动接口 */
    struct device_t * (*probe)(struct driver_t * drv, struct dtnode_t * dt);
    void (*remove)(struct device_t * dev);
    void (*suspend)(struct device_t * dev);
    void (*resume)(struct device_t * dev);
};

/* 根据名称搜索驱动 */
struct driver_t * search_driver(const char * name);
/* 注册一个驱动 */
bool_t register_driver(struct driver_t * drv);
/* 注销驱动 */
bool_t unregister_driver(struct driver_t * drv);
/* 探测设备树文件中描述的设备,根据设备树文件中设备顺序搜索驱动并注册设备 */
void probe_device(const char * json, int length, const char * tips);
/* 移除设备 */
void remove_device(struct device_t * dev);

#ifdef __cplusplus
}
#endif

#endif /* __DRIVER_H__ */
