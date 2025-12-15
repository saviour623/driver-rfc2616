#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <immintrin.h>

#define USE_SIMD
#define NOT(e) (!(e))
#define puti(i) fprintf(stderr, "%llu\n", (long long)(i))

#include <time.h>
#define START(t) ((t) = clock())
#define STOP(t) ((t) = clock() - (t))
#define PRINT(t) (fprintf(stderr, "%f\n", (t) / (double)CLOCKS_PER_SEC))

struct Tobject
{
	uint8_t __obmeta__[16];
	uint8_t *__obdata__[13];
	uint8_t *__obnext__;
};

static __inline__ __attribute__((always_inline, pure)) int objectGetId(const char *key)
{
	uint32_t mask = 0, e = __builtin_strlen(key);

	if (e < 4)
	{
		mask = 5381;
		switch (e)
		{
		case 3:
			mask = (mask << 5 + mask) + *key++;
		case 2:
			mask = (mask << 5 + mask) + *key++;
		}
		return ((mask << 5 + mask) + *key) % 13;
	}
	mask = *(uint16_t *)key;
	mask |= ((uint32_t)*(uint16_t *)(key + e - 3) << 16);
	mask = ((mask * 0x5a2b0f0f) + (uint32_t)((key[(uint32_t)(e * 0.56f)]) << 3));

	return (mask + (0xe2e3e11 * key[e - 1])) % 13;
}

static const __inline__ __attribute__((always_inline)) int8_t ObjectFindKey(const struct Tobject *object, const uint8_t *key, const uint8_t id)
{
	uint8_t *meta__ = object->__obmeta__, **obdata__ = object->__obdata__, *obkey __attribute__((unused)) = NULL;
	//*(uint64_t *)meta__ &= 0xffffffffffULL;
#ifdef USE_SIMD
	uint16_t mask = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128((void *)meta__), _mm_set1_epi8(id))) >> 3, idx = 0;
	uint8_t *_key;

	do
	{
		_key = key;
		idx = __builtin_ctz(mask);
		obkey = obdata__[idx];
		mask &= (0b1111111111110u << idx);
	} while ((*obkey++ ^ *_key++ || (*obkey++ ^ *_key++) || strcmp(obkey, _key)) && mask);
	return idx;
#else
	uint16_t idx = 0;
	uint8_t *_key;
	for (idx = 0; idx < 16; idx++)
	{
		_key = key;
		obkey = obdata__[idx];
		if (meta__[idx] ^ id || strcmp(obkey, _key)))
		break;
	}
	return idx;
#endif
	return -1;
}

int main(void)
{
	clock_t t;
	int8_t idx;
	struct Tobject *object = malloc(sizeof(struct Tobject));

	if (object == NULL)
	{
		perror("malloc");
		exit(-1);
	}

	char *keys[14] = {
		"063dyjuy",
		"070462",
		"085tzzqi",
		"10th",
		"1",
		"11235813",
		"12qwaszx",
		"13576479",
		"135790",
		"142536",
		"142857",
		"147258",
		"14725836",
		"9678787"};

	for (int i = 0; i < 13; i++)
	{
		(object->__obdata__)[i] = keys[i];
	}

	uint8_t m[16];
	uint16_t k;
	memcpy(object->__obmeta__, (uint8_t[]){4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4}, 16);
	//_mm_storeu_si128((void *)(object->__obmeta__), _mm_set_epi8(0, 0, 0, 4, 0, 0, 4, 4,6, 6, 5, 2, 4, 4, 0, 5));

	volatile uint64_t bb = 0;

	bb = 0xffffffffffffffffULL; // 0b111111111111111111111111111111111111 1111 1111 1111 0000 1111 1111 1111

#define add(bb, i, d) (bb ^ (0b1111u << ((!!(i) << 2) << (i - NOT(NOT(i)))))) | (d << ((!!(i) << 2) << (i - NOT(NOT(i)))))
#define clr(bb, i) (bb ^ (0b1111u << (1 << i - 1)))
#define ObjectSetId(bf, idx, id)                            \
	do                                                      \
	{                                                       \
		const uint16_t shf = (NOT(NOT(idx)) << 2) << (idx); \
		(bf) = ((bf) ^ (0xfu << shf)) | ((id) << shf);      \
	} while (0)

	int16_t p = 5;
	START(t);
	for (int i = 0; i < 10000; i++)
		bb = add(bb, p, i & 15);
	// idx = ObjectFindKey(object, "1", 4);
	STOP(t);
	printf("%u\n", idx);
	PRINT(t);
#if 0
   START(t);
  for (int i = 0; i < 10000; i++)
	;//bb = add2(bb, 5, 15);
	 // idx = ObjectFindKey(object, "1", 4);
   STOP(t);
   printf("%u\n", idx);
   PRINT(t);
#endif
	uint64_t i = UINT64_MAX;

	// i = add(i, 3, 15);
	// i= add(i, 3, 15);

	// ObjectSetId(i, 0, 0);
	puti(add(i, 1, 0));
	free(object);
	return 0;
}
