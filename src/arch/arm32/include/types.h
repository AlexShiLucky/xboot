#ifndef __ARM32_TYPES_H__
#define __ARM32_TYPES_H__

#define FIELD_BIT00_07  \
    u8_t  b0  :1;   \
    u8_t  b1  :1;   \
    u8_t  b2  :1;   \
    u8_t  b3  :1;   \
    u8_t  b4  :1;   \
    u8_t  b5  :1;   \
    u8_t  b6  :1;   \
    u8_t  b7  :1;
#define FIELD_BIT08_15  \
    u8_t  b8  :1;   \
    u8_t  b9  :1;   \
    u8_t  b10 :1;   \
    u8_t  b11 :1;   \
    u8_t  b12 :1;   \
    u8_t  b13 :1;   \
    u8_t  b14 :1;   \
    u8_t  b15 :1;
#define FIELD_BIT16_31  \
    u8_t  b16 :1;   \
    u8_t  b17 :1;   \
    u8_t  b18 :1;   \
    u8_t  b19 :1;   \
    u8_t  b20 :1;   \
    u8_t  b21 :1;   \
    u8_t  b22 :1;   \
    u8_t  b23 :1;   \
    u8_t  b24 :1;   \
    u8_t  b25 :1;   \
    u8_t  b26 :1;   \
    u8_t  b27 :1;   \
    u8_t  b28 :1;   \
    u8_t  b29 :1;   \
    u8_t  b30 :1;   \
    u8_t  b31 :1;
#define FIELD_BIT32_63  \
    u8_t  b32 :1;   \
    u8_t  b33 :1;   \
    u8_t  b34 :1;   \
    u8_t  b35 :1;   \
    u8_t  b36 :1;   \
    u8_t  b37 :1;   \
    u8_t  b38 :1;   \
    u8_t  b39 :1;   \
    u8_t  b40 :1;   \
    u8_t  b41 :1;   \
    u8_t  b42 :1;   \
    u8_t  b43 :1;   \
    u8_t  b44 :1;   \
    u8_t  b45 :1;   \
    u8_t  b46 :1;   \
    u8_t  b47 :1;   \
    u8_t  b48 :1;   \
    u8_t  b49 :1;   \
    u8_t  b50 :1;   \
    u8_t  b51 :1;   \
    u8_t  b52 :1;   \
    u8_t  b53 :1;   \
    u8_t  b54 :1;   \
    u8_t  b55 :1;   \
    u8_t  b56 :1;   \
    u8_t  b57 :1;   \
    u8_t  b58 :1;   \
    u8_t  b59 :1;   \
    u8_t  b60 :1;   \
    u8_t  b61 :1;   \
    u8_t  b62 :1;   \
    u8_t  b63 :1;

#define FIELD_BITS08    \
        FIELD_BIT00_07
#define FIELD_BITS16    \
        FIELD_BIT00_07  \
        FIELD_BIT08_15
#define FIELD_BITS32    \
        FIELD_BIT00_07  \
        FIELD_BIT08_15  \
        FIELD_BIT16_31
#define FIELD_BITS64    \
        FIELD_BIT00_07  \
        FIELD_BIT08_15  \
        FIELD_BIT16_31  \
        FIELD_BIT32_63

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char             s8_t;
typedef unsigned char           u8_t;

typedef signed short            s16_t;
typedef unsigned short          u16_t;

typedef signed int              s32_t;
typedef unsigned int            u32_t;

typedef signed long long        s64_t;
typedef unsigned long long      u64_t;

typedef float                   fp32_t;
typedef double                  fp64_t;

typedef signed long long        intmax_t;
typedef unsigned long long      uintmax_t;

typedef signed int              ptrdiff_t;
typedef signed int              intptr_t;
typedef unsigned int            uintptr_t;

typedef unsigned int            size_t;
typedef signed int              ssize_t;

typedef signed int              off_t;
typedef signed long long        loff_t;

typedef signed int              bool_t;
typedef signed int              register_t;
typedef unsigned int            irq_flags_t;

typedef unsigned int            virtual_addr_t;
typedef unsigned int            virtual_size_t;
typedef unsigned int            physical_addr_t;
typedef unsigned int            physical_size_t;

typedef struct {
    volatile int counter;
} atomic_t;

typedef struct {
    volatile int lock;
} spinlock_t;

typedef union _BIT8 {
    s8_t    s8;
    u8_t    u8;
    struct {
        FIELD_BITS08
    } bits;
} BIT8, bit8_t;

typedef union _BIT16 {
    s16_t   s16;
    u16_t   u16;
    s8_t    s8[2];
    u8_t    u8[2];
    struct {
        FIELD_BITS16
    } bits;
} BIT16, bit16_t;

typedef union _BIT32 {
    s32_t   s32;
    u32_t   u32;
    s16_t   s16[2];
    u16_t   u16[2];
    s8_t    s8[4];
    u8_t    u8[4];
    fp32_t  fp32;
    struct {
        FIELD_BITS32
    } bits;
} BIT32, bit32_t;

typedef union _BIT64 {
    s64_t   s64;
    u64_t   u64;
    s32_t   s32[2];
    u32_t   u32[2];
    s16_t   s16[4];
    u16_t   u16[4];
    s8_t    s8[8];
    u8_t    u8[8];
    fp64_t  fp64;
    struct {
        FIELD_BITS64
    } bits;
} BIT64, bit64_t;

#ifdef __cplusplus
}
#endif

#undef FIELD_BIT00_07
#undef FIELD_BIT08_15
#undef FIELD_BIT16_31
#undef FIELD_BITS08
#undef FIELD_BITS16
#undef FIELD_BITS32
#undef FIELD_BITS64

#endif /* __ARM32_TYPES_H__ */
