#ifndef __JSON_H__
#define __JSON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stdint.h>

struct json_value_t;
struct json_object_entry_t;

enum json_type_t {
	JSON_NONE		= 0,    // JSON
	JSON_OBJECT		= 1,    // JSON对象类型
	JSON_ARRAY		= 2,    // JSON数组类型
	JSON_INTEGER	= 3,    // JSON整数类型
	JSON_DOUBLE		= 4,    // JSON浮点数类型
	JSON_STRING		= 5,    // JSON字符串类型
	JSON_BOOLEAN	= 6,    // JSON布尔类型
	JSON_NULL		= 7,    // JSON NULL类型
};

/* JSON值 */
struct json_value_t {
	struct json_value_t * parent;
	enum json_type_t type;

	union {
        /* 布尔值 */
		int boolean;
        /* 整型数 */
		int64_t integer;
        /* 浮点数 */
		double dbl;

        /* 字符串 */
		struct {
			unsigned int length;
			char * ptr;
		} string;

        /* 对象 */
		struct {
			unsigned int length;
			struct json_object_entry_t * values;
		} object;

        /* 数组 */
		struct {
			unsigned int length;
			struct json_value_t ** values;
		} array;
	} u;

	union {
		struct json_value_t * next_alloc;
		void * object_mem;
	} reserved;
};

/* JSON对象实体,包含名称和对象 */
struct json_object_entry_t {
	char * name;
	unsigned int name_length;
	struct json_value_t * value;
};

struct json_value_t * json_parse(const char * json, size_t length, char * errbuf);
void json_free(struct json_value_t * value);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_H__ */
