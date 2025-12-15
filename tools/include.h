/* MVPG utils Header for including compatible system and standard Libraries, macro helpers and debugging functionalies
Copyright (C) 2025 Michael Saviour

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MVPG_INCLUDE_H
#define MVPG_INCLUDE_H


/***********************************************************************

* C STANDARD/SYSTEM HEADER

***********************************************************************/

#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>
#include <errno.h>
#include <assert.h>


/***********************************************************************

* COMPILER SPECIFIC ATTRIBUTES/INTRINSICS

***********************************************************************/
#if defined(__GNUC__) || defined(__clang__)
    #define __GNUC_LLVM__ 1
#elif defined(_MSC_VER) || defined(WIN32) || defined(_win32)
    #define __WINDOWS__   1
#else
    #define __WINDOWS__ 0
    #define __GNUC_LLVM__ 0
#endif
#if defined(__STDC__) && (__STDC_VERSION >= 201112L)
    #define __STDC_GTEQ_11__
#endif

#if __GNUC_LLVM__
    #define __FORCE_INLINE__ __attribute__((always_inline))
#elif __WINDOWS__
    #define __FORCE_INLINE__ __forceinline
#else
    #define __FORCE_INLINE__
#endif

#if __GNU_LLVM__
    #define __MAY_ALIAS__   __attribute__((may_alias))
    #define __MB_UNUSED__   __attribute__((unused))
    #define __WARN_UNUSED__ __attribute__ ((warn_unused_result))
    #define __NONNULL__     __attribute__((nonnull))
    #define LIKELY___(x, p) __builtin_expect(x, p)
    #define TYPEOF(T)       __typeof__(T)
#else
    #define __MAY_ALIAS__
    #define __MB_UNUSED__
    #define __WARN_UNUSED__
    #define __NONNULL__
    #define LIKELY__(x, p) (x)
#endif
#define INLINE(T) __inline__ __FORCE_INLINE__ T
#define __STATIC_FORCE_INLINE_F static __inline__ __FORCE_INLINE__


/***********************************************************************

* TOOL MACROS

***********************************************************************/

#include "../macro/macro.h"

#define MvpgMacro_Vaopt(...)        MAC_VA_OPT__(__VA_ARGS__)
#define MvpgMacro_Select(A, B, ...) MAC_SELECT__(A, B, __VA_ARGS__)
#define MvpgMacro_Concat(A, B)      CAT__(A, B)
#define MvpgMacro_Stringify(S)      #S
#define MvpgMacro_Ignore(...)       (void)(__VA_ARGS__)


/***********************************************************************

* MATH

***********************************************************************/

#define     MOD2(n, m) ((n) & ((m) - 1)) /* n % m (m is a power of 2) */
#define   MODP2(n, p2) MOD2(n, 1ULL << p2) /* N % 2^p2 */
#define PRVMULP2(n, m) ((n) - ((n) & ((m) - 1))) /* (multiple of 2^m) < n */
#define   NXTMUL(n, m) (((n) + ((m) - 1)) & ~((m) - 1)) /* {(multiple m) >= n (m is a power of 2)} */
#define NXTMULP2(n, m) ((((n) >> m) + 1) << m) /* {n < (multiple of 2^m) > n} */


/***********************************************************

 * SAFE INTEGER ARITHMETIC

************************************************************/

/* SAFE_MUL_ADD (__bMulOverflow,  __bAddOverflow, safeMulAdd)
*  Returns 0 if operation succeeded
*/
#if __GNUC_LLVM__
    #define __bMulOverflow(a, b, c) __builtin_mul_overflow(a, b, c)
    #define __bAddOverflow(a, b, c) __builtin_add_overflow(a, b, c)
#elif __WINDOWS__
/* WINDOWS KENRNEL API FOR SAFE ARITHMETIC */
    #include <ntintsafe.h>
    #define __bAddOverflow(a, b, c) (RtlLongAdd(a, b, c) == STATUS_INTEGER_OVERFLOW)
    #define __bMulOverflow(a, b, c) (RtlLongMul(a, b, c) == STATUS_INTEGER_OVERFLOW)
#else
#define __bAddOverflow(a, b, c) !( ((a) < (ULONG_MAX - (b)))) && ((*(c) = (a) + (b)), 0)
    #define __bMulOverflow(a, b, c) !( !(((a) > (ULONG_MAX>>1)) || ((b) > (ULONG_MAX>>1))) && ((*(c) = a * b), 0)
#endif

/* Add */
    __STATIC_FORCE_INLINE_F unsigned long int __bsafeUnsignedAddl(unsigned long int a, unsigned long int b) {
      assert(( "INTEGER OVERFLOW -> ADD", __bAddOverflow(a, b, &b) == 0 ));

      return b;
    }

/* Mul */
__STATIC_FORCE_INLINE_F unsigned long int __bsafeUnsignedMull(unsigned long int a, unsigned long int b) {
  assert(( "INTEGER OVERFLOW -> MUL", __bMulOverflow(a, b, &b) == 0 ));

  return b;
}

/* Add and Mul (unsigned long) */
__STATIC_FORCE_INLINE_F unsigned long int __bsafeUnsignedMulAddl(unsigned long int a, unsigned long int b, unsigned long int c) {

  assert(( "INTEGER OVERFLOW -> MUL_ADD", !__bMulOverflow(a, b, &b) && !__bAddOverflow(b, c, &c) ));

  return c;
}

/***********************************************************************

* FUNCTION PROTOTYPES FROM INCLUDE.C

***********************************************************************/
/* Similar to assert */
void _debugAssert(const char *, const unsigned long int, const char *, const char *, const char *);

/* Copy n bytes from src to dest; deviates from strlcpy in that dest is updated to dest + n, on return  */
size_t MvpgInclude_strlcpy(char **, char *, size_t);

/* Convert integer to string */;
uintmax_t MvpgInclude_Itoa(uintmax_t, char *, uint8_t, uint8_t);

 /***********************************************************************

* DEBUG

***********************************************************************/
#define PASS (void)0

#ifdef MVPG_NDEBUG
    #define debugAssert(...)
#else
    #define debugAssert(expr, ...) (\
 (expr) || (_debugAssert(__FILE__, __LINE__, __FUNCTION__, #expr, MvpgMacro_Select((__VA_ARGS__), "", __VA_ARGS__)), 1) \
				    )
#endif

#define outs(...) puts(__VA_ARGS__)
#define puti(i) printf("%llu\n", (long long int)(i))
#define putd(i) printf("%lld\n", (long long int)(i))
#define putf(i) printf("%.20f\n", (double)(i))
#endif
