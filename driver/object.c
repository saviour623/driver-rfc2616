#include <stdint.h>
#include <immintrin.h>

#define OBJDEF_SIZE ((16 * 2) + 16)

typedef struct __object __object;
typedef __object * tobject;

struct __object
{
    uint64_t __meta[2];
    void    *__dat;
    void    *__cache;
    struct   __object *__next;
    uint16_t __size;
}

 static struct __object * __attribute__((noinline, warn_unused)) __initobject(void)
{
    struct __object *object;

    if (object = calloc(sizeof(struct __object) + ((16 * 2) + 16)), 1) return NULL;
    object_setup_cache(object);
    object_setup_data (object);
    object->__next = NULL;

    return object;
}