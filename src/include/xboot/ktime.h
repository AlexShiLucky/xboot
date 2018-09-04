#ifndef __KTIME_H__
#define __KTIME_H__

#include <types.h>

typedef union {
    s64_t tv64;
} ktime_t;

#define KTIME_MAX           ((s64_t)~((u64_t)1 << 63))
#define KTIME_SEC_MAX       (KTIME_MAX / 1000000000ULL)

static inline ktime_t ktime_set(const s64_t s, const unsigned long ns)
{
    if((s >= KTIME_SEC_MAX))
        return (ktime_t) { .tv64 = KTIME_MAX };
    return (ktime_t) { .tv64 = (s64_t)s * 1000000000ULL + (s64_t)ns };
}

/* 比较两个ktime是否相等 */
static inline int ktime_equal(const ktime_t a, const ktime_t b)
{
    return (a.tv64 == b.tv64);
}

/* 比较两个ktime大小 */
static inline int ktime_compare(const ktime_t a, const ktime_t b)
{
    if(a.tv64 < b.tv64)
        return -1;
    if(a.tv64 > b.tv64)
        return 1;
    return 0;
}

/* 判断a是否在b之后 */
static inline int ktime_after(const ktime_t a, const ktime_t b)
{
    return ktime_compare(a, b) > 0;
}

/* 判断a是否在b之前 */
static inline int ktime_before(const ktime_t a, const ktime_t b)
{
    return ktime_compare(a, b) < 0;
}

/* a + b */
#define ktime_add(a, b)         ({ (ktime_t){ .tv64 = (a).tv64 + (b).tv64 }; })
/* a - b */
#define ktime_sub(a, b)         ({ (ktime_t){ .tv64 = (a).tv64 - (b).tv64 }; })
/* a + ns */
#define ktime_add_ns(kt, ns)    ({ (ktime_t){ .tv64 = (kt).tv64 + (ns) }; })
/* a - ns */
#define ktime_sub_ns(kt, ns)    ({ (ktime_t){ .tv64 = (kt).tv64 - (ns) }; })

/* 两个ktime相加,防溢出 */
static inline ktime_t ktime_add_safe(const ktime_t a, const ktime_t b)
{
    ktime_t kt = ktime_add(a, b);

    if((kt.tv64 < 0) || (kt.tv64 < a.tv64) || (kt.tv64 < b.tv64))
        kt = ktime_set(KTIME_SEC_MAX, 0);
    return kt;
}

/* a + us */
static inline ktime_t ktime_add_us(const ktime_t kt, const u64_t us)
{
    return ktime_add_ns(kt, us * 1000L);
}

/* a - us */
static inline ktime_t ktime_sub_us(const ktime_t kt, const u64_t us)
{
    return ktime_sub_ns(kt, us * 1000L);
}

/* a + ms */
static inline ktime_t ktime_add_ms(const ktime_t kt, const u64_t ms)
{
    return ktime_add_ns(kt, ms * 1000000L);
}

/* a - ms */
static inline ktime_t ktime_sub_ms(const ktime_t kt, const u64_t ms)
{
    return ktime_sub_ns(kt, ms * 1000000L);
}

/* ktime -> ns */
static inline s64_t ktime_to_ns(const ktime_t kt)
{
    return ((kt).tv64);
}

/* ktime -> us */
static inline s64_t ktime_to_us(const ktime_t kt)
{
    return (kt.tv64 / 1000L);
}

/* ktime -> ms */
static inline s64_t ktime_to_ms(const ktime_t kt)
{
    return (kt.tv64 / 1000000L);
}

/* 计算两个ktime相差us */
static inline s64_t ktime_us_delta(const ktime_t later, const ktime_t earlier)
{
    return ktime_to_us(ktime_sub(later, earlier));
}

/* 计算两个ktime相差ms */
static inline s64_t ktime_ms_delta(const ktime_t later, const ktime_t earlier)
{
    return ktime_to_ms(ktime_sub(later, earlier));
}

/* ns -> ktime */
static inline ktime_t ns_to_ktime(u64_t ns)
{
    static const ktime_t ktime_zero = { .tv64 = 0 };
    return ktime_add_ns(ktime_zero, ns);
}

/* us -> ktime */
static inline ktime_t us_to_ktime(u64_t us)
{
    static const ktime_t ktime_zero = { .tv64 = 0 };
    return ktime_add_us(ktime_zero, us);
}

/* ms -> ktime */
static inline ktime_t ms_to_ktime(u64_t ms)
{
    static const ktime_t ktime_zero = { .tv64 = 0 };
    return ktime_add_ms(ktime_zero, ms);
}

#endif /* __KTIME_H__ */
