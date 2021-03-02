#ifndef __DTREE_H__
#define __DTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>
#include <string.h>
#include <json.h>

/* 设备树节点结构 */
struct dtnode_t {
    const char * name;
    physical_addr_t addr;
    struct json_value_t * value;
};

/* 获取设备节点名称,即获取@左侧部分,该名称对应驱动名称 */
const char * dt_read_name(struct dtnode_t * n);
/* 获取设备自动分配起始索引,即获取@右侧部分,默认从.0开始 */
int dt_read_id(struct dtnode_t * n);
/* 获取设备物理地址 */
physical_addr_t dt_read_address(struct dtnode_t * n);

/* 获取设备节点布尔型数据 */
int dt_read_bool(struct dtnode_t * n, const char * name, int def);
/* 获取设备节点整型数据 */
int dt_read_int(struct dtnode_t * n, const char * name, int def);
/* 获取设备节点长整型数据 */
long long dt_read_long(struct dtnode_t * n, const char * name, long long def);
/* 获取设备节点浮点型数据 */
double dt_read_double(struct dtnode_t * n, const char * name, double def);
/* 获取设备节点字符串类型数据 */
char * dt_read_string(struct dtnode_t * n, const char * name, char * def);
/* 获取设备节点8位无符号整型数据 */
u8_t dt_read_u8(struct dtnode_t * n, const char * name, u8_t def);
/* 获取设备节点16位无符号整型数据 */
u16_t dt_read_u16(struct dtnode_t * n, const char * name, u16_t def);
/* 获取设备节点32位无符号整型数据 */
u32_t dt_read_u32(struct dtnode_t * n, const char * name, u32_t def);
/* 获取设备节点64位无符号整型数据 */
u64_t dt_read_u64(struct dtnode_t * n, const char * name, u64_t def);
/* 获取设备节点json对象数据 */
struct dtnode_t * dt_read_object(struct dtnode_t * n, const char * name, struct dtnode_t * o);
/* 获取设备节点数组长度 */
int dt_read_array_length(struct dtnode_t * n, const char * name);
/* 获取设备节点数组中布尔型数据 */
int dt_read_array_bool(struct dtnode_t * n, const char * name, int idx, int def);
/* 获取设备节点数组中整型数据 */
int dt_read_array_int(struct dtnode_t * n, const char * name, int idx, int def);
/* 获取设备节点数组中长整型数据 */
long long dt_read_array_long(struct dtnode_t * n, const char * name, int idx, long long def);
/* 获取设备节点数组中浮点型数据 */
double dt_read_array_double(struct dtnode_t * n, const char * name, int idx, double def);
/* 获取设备节点数组中字符串型数据 */
char * dt_read_array_string(struct dtnode_t * n, const char * name, int idx, char * def);
/* 获取设备节点数组中8位无符号整型数据 */
u8_t dt_read_array_u8(struct dtnode_t * n, const char * name, int idx, u8_t def);
/* 获取设备节点数组中16位无符号整型数据 */
u16_t dt_read_array_u16(struct dtnode_t * n, const char * name, int idx, u16_t def);
/* 获取设备节点数组中32位无符号整型数据 */
u32_t dt_read_array_u32(struct dtnode_t * n, const char * name, int idx, u32_t def);
/* 获取设备节点数组中64位无符号整型数据 */
u64_t dt_read_array_u64(struct dtnode_t * n, const char * name, int idx, u64_t def);
/* 获取设备节点数组中json对象数据 */
struct dtnode_t * dt_read_array_object(struct dtnode_t * n, const char * name, int idx, struct dtnode_t * o);

#ifdef __cplusplus
}
#endif

#endif /* __DTREE_H__ */
