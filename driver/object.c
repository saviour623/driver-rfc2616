#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

struct __object
{
    uint64_t __meta[4];
    void    *__dat;
    void    *__cache;
    struct   __object *__np;
    uint16_t __size;
};

typedef struct { void *key, *value; } *__object_internal_p;

#define NOT(e) (!(e))
#define OBJ_BLOCK (sizeof(struct __object))
#define OBJ_FWDBLOCK (OBJ_BLOCK + ((16 * 2) + 16))
#define object_setup_internal(obj) \
    (((obj)->__dat) = (uint8_t *)(obj) + OBJ_BLOCK), ((obj)->__cache = ((uint8_t *)(obj)+OBJ_FWDBLOCK))

#define object_internal_get(obj, where) \
    (((__object_internal_p)(obj))[where])



__attribute__((noinline, warn_unused)) static struct __object *__init__(void)
{
    struct __object *object;

    if (NOT(object = calloc(OBJ_FWDBLOCK, sizeof(uint8_t))))
        return NULL;
    object->__np = NULL;
    object_setup_internal(object);

    return object;

}



static __inline__ __attribute__((always_inline, pure)) uint32_t __hash__(const void *key, const int len)
{
    // Implement MurmurHash32 for 32 bit hashes
    return 0;
}



static __attribute__((nonnull)) void __add__(struct __object *object, const void *__restrict key, const void *__restrict value)

{

    uint32_t where = __hash__(key, strlen(key)) & (object->__size - 1); // object data capacity is a maximum of 256 items

    if (NOT((object->__meta)[(0xff & -(object->__size > 0xff))] & (1ull << ((where >> 1) + !!(where & 0x3f)))) && __get_unused__(&where, object->__meta, object->__size > 0xff))
    {
        // resize object
    }

    object_internal_get(object, where).key   = key;
    object_internal_get(object, where).value = value;
    object_internal_cache(object, where);
    object->__size += 1;
}

static __attribute__((nonnull)) void __remove__(struct __object *object, const void *__restrict key)
{
    uint32_t where;

    if ((where = __find__(object, key)) < 0)
        return;
    return object_internal_rmcache(object, where);

}

static __attribute__((nonnull)) void *__getvalue__(struct __object *object, const void *__restrict key)
{
    uint32_t where;

    if ((where = __find__(object, key)) < 0)
        return;

    return object_internal_get(object, where).value;
}



__attribute__((noinline)) static struct __object *__del__(struct __object *object)
{

    free(object);
    return NULL;
}
