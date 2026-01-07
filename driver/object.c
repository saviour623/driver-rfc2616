#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <immintrin.h>

#if defined(WIN32) || defined(_MSV_VER)
#define WINSYS 1
#include <windows.h>
#include <windef.h>
#endif

struct __object
{
    uint64_t __meta[4];
    __object_internal_p __data;
    void *__cache;
    struct __object *__np;
    uint16_t __size;
};

typedef struct
{
    void *key, *value;
#if INCLUDE_HASH
    uint32_t hash;
#endif
} *__object_internal_p;

#define NOT(e) (!(e))
#define RET_FVALUE 1
#define RET_FIDX 0
#define PROBE_MAX (1 << 16) // number of probes before rehashing

#define OBJ_BLOCK (sizeof(struct __object))
#define OBJ_FWDBLOCK (OBJ_BLOCK + ((16 * 2) + 16))
#define OBJ_NULL 0
#define __objc_setup_internal__(obj) \
    (((obj)->__data) = (uint8_t *)(obj) + OBJ_BLOCK), ((obj)->__cache = ((uint8_t *)(obj) + OBJ_FWDBLOCK))

#define __meta__(objc) ((objc)->__meta)
#define __data__(objc) ((objc)->__data)
#define __cache__(objc) ((objc)->__cache)
#define __size__(objc) ((objc)->__size)
#define __null__(b, i) NOT(((b)[(i) >> 6]) & ((i) & 0x3fu))

#define __get__(objc, where) (((__object_internal_p)(objc))[where])
#define __value__(objc, where) (__get__(objc, where)).value
#define __key__(objc, where) (__get__(objc, where)).key
#define __setcache__(objc, where) (((uint8_t *)__cache__(objc))[where] = (((where) & 0xff) ^ 0xff))
#define __rsetcache__(objc, where) (((uint8_t *)__cache__(objc))[where] = OBJ_NULL)

#if INCLUDE_HASH
#define __get_hash__(objc, where) (__get__(objc, where)).hash
#define __compare_hash(hash_1, hash_2) NOT((hash_1) ^ (hash_2))
#else
#define __get_hash__(...) 0
#define __compare_hash(...) 0
#endif

#define CAT(_1, _2) _1##_2
#define EXPAND(...) __VA_ARGS__

#define __set_HASH(arg)
#define __set_ALL()
#define __set_CACHE()

#define __set(op1, arg1, op2, arg2, op3, arg3, op4, arg4, ...) \
    (CAT(__set_, op1)(arg1),                                   \
     CAT(__set_, op2)(arg2),                                   \
     CAT(__set_, op3)(arg3),                                   \
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

#if (__MINGW64__) || defined(__clang__) || defined(__GCC__)
#define __scan_reverse(mask) __builtin_ctzll(mask)
#elif defined(WINSYS)
#ifdef _bit_scan_reverse
#define __scan_reverse(mask) _bit_scan_reverse(mask)
#else
extern __inline__ __forceinline unsigned long __scan_reverse(const uint64_t mask)
{
    unsigned long idx;
    return (_BitScanReverse64(&idx, mask), idx);
}
#endif
#endif

/*
 * FUNC: @__cache_id__
 * DESC: return the cache ID of @hash
 */
static __inline__ __attribute__((always_inline, pure)) uint8_t __cache_id__(const uintmax_t hash)
{
    return ((hash & 0xffffu) - ((hash & 0xffffu) * 0xff01u) >> 24) + 1; // low_16_hash % 251 (add 1 to avoid zero - clearbit)
}

/*
 * FUNC: @__find__
 */
static __attribute__((nonnull)) void *__find__(const struct __object const *object, const char *__key, uint8_t return_Policy)
{
#ifdef __AVX256__
    typedef __m256i intx8_t;
    typedef uint32_t mask_t;
#define RDWORD 32
#define RDWORD_P2 5
#define RDWORD_BFOR 128
#define SHIFT_IDX 0
#define U_ONE (mask_t)1
#define __broadcast_byte(x) _mm256_set1_epi8((uint8_t)(x))
#define __testeq__(v, x) _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si128((const __m256i *)(v)), (X)))
#elif __SSE2__
    typedef __m128i intx8_t;
    typedef uint16_t mask_t;
#define RDWORD 16
#define RDWORD_P2 4
#define RDWORD_BFOR 128
#define SHIFT_IDX 0
#define U_ONE (mask_t)1
#define __broadcast_byte(x) _mm_set1_epi8((uint8_t)(x))
#define __testeq__(v, x) _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128((const __m128i *)(v)), (x)))
#elif __UINT64__
    typedef uint64_t intx8_t;
    typedef intx8_t mask_t;
#define RDWORD 8
#define RDWORD_P2 3
#define RDWORD_BFOR 64
#define SHIFT_IDX 3
#define U_ONE 1ull
#define __broadcast_byte(x) ((x) * 0x101010101010101ull)
#define __test_zero_fast(v) (bool)(((v) - 0x101010101010101ull) & (~(v) & 0x8080808080808080ull))
#define __test_zero_wi(v) ((((v) - 0x1000100010001ull) | ((v) - 0x100010001000100ull)) & (~(v) & 0x8080808080808080ull));
#define __test_eq_8(v, x) (__test_zero_wi((v) ^ ((x) * 0x101010101010101ull)))
#define __test_eq_8_precomp(v, px) (__test_zero_wi((v) ^ (px)))

    extern __inline__ __attribute__((gnu_inline, always_inline, pure)) uint64_t __testeq_fn(const uint64_t fv, const uint64_t mask)
    {
        return __test_eq_8_precomp(fv, mask);
    }
#define __testeq__(v, x) __testeq_fn(((uint64_t *)(v))[0], x)
#else
#error UNIMPLEMENTED
#endif
#define ROUNDDWN_MULP(n, pow_2) ((n) - ((n & (pow_2))))
#define ROUNDUP_MULP(n, pow_2) (((n) + ((pow_2) - 1)) & ~((pow_2) - 1))
#define __read_next_cache__(cache) (++cache)

    const intx8_t *cache = NULL;
    __object_internal_p tag = NULL;

    mask_t mask = 0;
    uint32_t hash = __hash__(__key, strlen(__key));
    uint32_t where = (hash & (__size__(object) - 1));
    _Alignas(RDWORD) const intx8_t mulx8_hash = __broadcast_byte(__cache_id__(hash));

    if (0)
    {
        // TODO: if position already has what we are looking for, return it
        return where;
    }

    where = ROUNDDWN_MULP(where, RDWORD_BFOR); // read cache bytes aligned to sizeof (intx8_t) and atmost 2*(sizeof (intx8_t)) before position
    cache = __cache__(object) + where;
    tag = __data__(object) + where;

    __builtin_prefetch(cache + 1);

    for (uint32_t i = (ROUNDUP_MULP(__size__(object), RDWORD) - where) >> RDWORD_P2; i--;)
    {
        mask = __testeq__(cache, mulx8_hash);
        while (mask)
        {
            where = __scan_reverse(mask);
            mask ^= (U_ONE << where);
            if (__compare_hash(tag[(where >> SHIFT_IDX)].hash, hash) && NOT(strcmp(tag[(where >> SHIFT_IDX)].key, __key)))
                return where;
        }
        tag = tag + RDWORD;
        __read_next_cache__(cache);
    }
    return NULL;
}

static uint32_t __get_unused__(uint64_t *switch_ctrl, uint32_t *from)
{
    uint32_t __from = ROUNDDWN_MULP(*from, RDWORD_BFOR);

    // TODO: finish this later
    switch_ctrl += __from >> 8;

    for (uint32_t i = 0; i < PROBE_MAX >> 8; i++)
    {
        if (__builtin_expect(switch_ctrl[i] ^ 0xffffffffffffffffull, 1))
            return __scan_reverse(switch_ctrl[i]);
    }

    return __from;
}

static __attribute__((nonnull)) void __add__(struct __object *object, const void *__restrict key, const void *__restrict value)
{
    uint32_t where = __hash__(key, strlen(key)) & (__size__(object) - 1); // object data capacity is a maximum of 256 items

    if (__null__(__meta__(object), where) && __get_unused__(__meta__(object), &where))
    {
        // resize object
    }

    __key__(object, where) = key;
    __value__(object, where) = value;

    __setcache__(object, where);
    __meta__(object)[where] &= (1u << (where & 0x3f));
    __size__(object) += 1;
}

static __attribute__((nonnull)) void __remove__(struct __object *object, const void *__restrict key)
{
    uint32_t where;

    if ((where = __find__(object, key, RET_FIDX)) < 0)
    {
        // Error: No such element
    }
    return __rsetcache__(object, where);
}

static __inline__ __attribute__((nonnull, always_inline)) void *__getvalue__(const struct __object *object, const void *__restrict __key)
{
    return __find__(object, __key, RET_FVALUE);
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

/*

// // // //
// // // //
// // // //
// // // //

11

*/