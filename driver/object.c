#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <immintrin.h>

struct __object
{
    uint64_t __meta[4];
    void *__dat;
    void *__cache;
    struct __object *__np;
    uint16_t __size;
};

typedef struct
{
    void *key, *value;
} *__object_internal_p;

#define NOT(e) (!(e))
#define OBJ_NULL 0xff
#define OBJ_BLOCK (sizeof(struct __object))
#define OBJ_FWDBLOCK (OBJ_BLOCK + ((16 * 2) + 16))
#define __objc_setup_internal__(obj) \
    (((obj)->__dat) = (uint8_t *)(obj)+OBJ_BLOCK), ((obj)->__cache = ((uint8_t *)(obj)+OBJ_FWDBLOCK))

#define __meta__(objc)  ((objc)->__meta)
#define __cache__(objc) ((objc)->__cache)
#define __size__(objc)  ((objc)->__size)
#define __null__(b, i)  NOT(((b)[(i) >> 6]) & ((i) & 0x3fu))

#define __get__(objc, where)   (((__object_internal_p)(objc))[where])
#define __value__(objc, where) (__get__(objc, where)).value
#define __key__(objc, where)   (__get__(objc, where)).key
#define __setcache__(objc, where)  (((uint8_t *)__cache__(objc))[where]=(((where)&0xff)^0xff))
#define __rsetcache__(objc, where) (((uint8_t *)__cache__(objc))[where]=OBJ_NULL)

#define CAT(_1, _2) _1 ## _2
#define EXPAND(...) __VA_ARGS__

#define __set_HASH(arg)
#define __set_ALL()
#define __set_CACHE()

#define __set(op1, arg1, op2, arg2, op3, arg3, op4, arg4, ...)\
    (CAT(__set_, op1)(arg1),\
    CAT(__set_, op2)(arg2),\
    CAT(__set_, op3)(arg3),\
    CAT(__set_, op4)(arg4))

__attribute__((noinline, warn_unused)) static struct __object *__init__(void)
{
    struct __object *object;

    if (NOT(object = calloc(OBJ_FWDBLOCK, sizeof(uint8_t))))
        return NULL;

    object->__np = NULL;
    __objc_setup_internal__(object);
    return object;
}

static __inline__ __attribute__((always_inline, pure)) uint32_t __hash__(const void *key, const int len)
{
    // Implement MurmurHash32 for 32 bit hashes
    return 0;
}

static __attribute__((nonnull)) void __add__(struct __object *object, const void *__restrict key, const void *__restrict value)
{
    uint32_t where = __hash__(key, strlen(key)) & (__size__(object) - 1); // object data capacity is a maximum of 256 items

    if (__null__(__meta__(object), where) && __get_unused__(&where, __meta__(object)))
    {
        // resize object
    }

    __key__  (object, where) = key;
    __value__(object, where) = value;

    __setcache__(object, where);
    __meta__(object)[where] &= (1u << (where & 0x3f));
    __size__(object) += 1;
}

static __attribute__((nonnull)) __object_internal_p __find__(const struct __object const *object, const void *__key)
{
#ifdef __AVX256__
#elif __SSE2__
#define __load_x16_8(x) __mm_set
#elif __BIT64__
#define __test_zero_fast(v) (bool)(((v) - 0x101010101010101ull) & (~(v) & 0x8080808080808080ull))
#define __test_zero_wi(v) ((((v) - 0x1000100010001ull) | ((v) - 0x100010001000100ull)) & (~(v) & 0x8080808080808080ull));
#define __test_eq_8(v, x) (__test_zero_wi((v) ^ ((x) * 0x101010101010101ull)))
#define __test_eq_8_precomp(v, px) (__test_zero_wi((v) ^ (px)))
#define __test_eq(v, p) __test_eq_8_precomp(v, p)
#else
#error OBJECT-FIND IS UNIMPLEMENTED
#endif
    struct __object *__objectp = object;
    uint64_t hash = __hash__(__key, strlen(__key)) & (__size__(__objectp) - 1);
    uint64_t mulx8_hash = ((hash&0xffull)^0xffull) * 0x101010101010101ull;
    uint64_t mask = 0;
    
    for (uint32_t i; (mask = __test_eq_8_precomp(((uint64_t *)__cache__(__objectp))[i]), mulx8_hash); i++)
        {

        }
}
static __attribute__((nonnull)) void __remove__(struct __object *object, const void *__restrict key)
{
    uint32_t where;

    if ((where = __find__(object, key)) < 0)
        return;
    return __rsetcache__(object, where);
}

static __attribute__((nonnull)) void *__getvalue__(struct __object *object, const void *__restrict key)
{
    uint32_t where;

    if ((where = __find__(object, key)) < 0)
        return;
    return __value__(object, where);
}

__attribute__((noinline)) static struct __object *__del__(struct __object *object)
{
    free(object);
    return NULL;
}

int main(void)
{
    struct __object *obj = __init__();
    __del__(obj);

    return 0;
}