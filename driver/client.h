#ifndef HTTP_CLIENT
#define HTTP_CLIENT

#if defined(WIN32) || defined(_MSC_VER) || defined(MINGW)
    #define WIN_SUPPORT
    #include <winsocket.h>
#elif defined(__unix__) || defined(__linux__) || defined(linux)
    #define UNIX_SUPPORT
    #include <netdb.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <unistd.h>
#else
    #error "no support for this system"
#endif
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

// TODO: Test for support for atleast ss2 instructions
#define USE_SIMD

#define WEBDR_PROTOCOL IPPROTO_TCP
#define WEBDR_SOCKTYPE SOCK_STREAM

#define WEDR_HTTP_PROTOCOL "HTTP/1.1"
#define WEBDR_LOCALHOST_I6 "::1"
#define WEBDR_LOCALHOST_I4 "127.0.0.1"

// Allow domain hint to be used strictly as the domain type
#ifdef WEBDR_STRICT_INET
// disallow fallback loop to generic in address searching
    #define WEBDR_SET_AFNET(af, hint, def) (((af) = (hint)), WEBDR_DOMRESET)
#else
#define WEBDR_SET_AFNET(af, hint, def)\
  ((af) = (def), (hint) != 0 ? (hint) : WEBDR_DOMRESET)
#endif

// TODO: Move safeUnsignedSize_Add and alignUp to include.h
#define safeUnsignedSize_Add(a, b, c) ( ((a) < (SIZE_MAX - (b))) && ((*(c) = (a) + (b)), 1) )
#define alignUp(n, p2) (((n) + ((p2) - 1)) & ~((p2) - 1))

#define WEBDR_APPEND_DELIM(buf, n) (((buf)[n] = CR), ((buf)[n + 1] = LF), 2)

#define ERROR_socket(...)      < 0) && Debug(WEBDR_EOSOCK
#define ERROR_bind(...)        < 0) && Debug(WEBDR_EOBIND
#define ERROR_close(...)       < 0) && Debug(WEBDR_CLOSE
#define ERROR_webdriverMalloc(...)  == NULL) && Debug(WEBDR_EOMEM
#define ERROR_webdriverRealloc(...) ERROR_webdriverMalloc(__VA_ARGS__)
#define ERROR_getaddrinfo(...) != 0) && Debug(0, gai_strerror(stat)

#define WerrorOccured(funcall, stat, ...)			\
  if (((stat = funcall) ERROR_##funcall, __VA_ARGS__))

#ifdef NDEBUG
    #define Debug(...) true
#else
    #include <stdio.h>
    #define Debug(symbol, ...) fprintf(stderr, "%s\n", "http error")
#endif

#define webdriverStrerror(err) "error"

#define JMP(label) goto label
#define LOCATION(label) label: (void)0
#define NOT(e) !(e)
#define ASSERT(...) assert((__VA_ARGS__))

#if defined(_GNU_SOURCE) || defined(__GNUC__) || defined(__gcc__) || defined(__clang__)
#define HIDDEN() __attribute__((visibility("hidden")))
#endif
#define _LOCK() //TODO: Implement a thread locking mechanism
#define _UNLOCK()

typedef struct {
  char *header;
  char *body;
} Wedbriver_MsgContent;

typedef struct Webdriver_Client__ Webdriver_Client__;
typedef struct Webdriver_Client__ * Webdriver_Client;

struct Webdriver_Client__ {
  struct sockaddr_storage addr__;
  char *buf__;
  size_t bufsize__;
  size_t bufc__;
  int errno__;
  const int sock__;
  _Bool malloc__;
  _Bool hascmd__;
};

#define WEBDR_BASE_SIZE sizeof (Webdriver_Client__)
#define WEBDR_DEF_MALLOC_SIZE 2048
#define WEBDR_DEF_MALLOC_GRW  1024

#define webdriverObjectTabCap 14
#define webdriverObjectCacheMaxCap  7
#define webdriverObjecAllocSize 1024

#define webdriverMemoryPoolMinAlloc (sizeof(void *))
#define webdriverMemoryPoolMetaSize 2
#define webdriverSizeofMemoryPool sizeof (struct Webdriver_TMemoryPool__)
#define webdriverMemoryPoolSize 49152
#define webdriverMemoryPoolMaxAlloc (webdriverMemoryPoolSize - webdriverSizeofMemoryPool - webdriverMemoryPoolMetaSize - 1)

typedef struct {
  char    *host;
  uint16_t port;
  uint16_t domain;
  _Bool    server;
} Webdriver_Config;

struct Webdriver_Itoabf
{
  uint8_t ibf[22];
  uint8_t len;
};

enum {
	  OBJECT,
	  ARRAY,
	  STRING,
	  NUMBER,
	  BOOLEAN
};

enum {
      MAX_NERROR     = 20,
      MIN_NERROR     = 0,
      WEBDR_DOMRESET = 0,
      WEBDR_EOMEM    = 0,
      WEBDR_EOSOCK,
      WEBDR_EOBIND,
      WEBDR_CLOSE,
      WEBDR_ADDRINFO,
      WEBDR_EOMETHOD,
      WEDR_EONOTEMPT, // attempt to reset buffer already allocated
	  WEBDR_INCOMPLETE,
      WEBDR_SUCCESS,
      WEBDR_FAILURE,
};

enum {
	  GET,
	  POST,
	  DELETE
};

enum {
      CR = '\r',
      LF = '\n',
      SP = ' ',
      COL = ':',
      TAB = '\t',
      EOC = '\0',
      EEOF
};

static const void *WDHttpMethodStr[3] = {
					   [ GET ]    = "GET",
					   [ POST ]   = "POST",
					   [ DELETE ] = "DELETE"
};

Webdriver_Client webdriverCreateClient( const Webdriver_Config * );
void   webdriverDestroyClient( Webdriver_Client );
static bool   webdriverSupportedMethods( const int );
static bool   webdriverError ( const void * );
static void   webdriverPerror( const void * );
void * webdriverSetbuf( Webdriver_Client, void *, const size_t );
void * webdriverUnsetbuf( Webdriver_Client );
const void * webdriverSetHttpCmd ( Webdriver_Client, const int, const void * );
const void * webdriverAddHttpHeader( Webdriver_Client, const void * __restrict, const void * __restrict );
void   webdriverShowHttpHeaders( Webdriver_Client );
size_t webdriverBufferUsed( Webdriver_Client client );

extern __inline__ __attribute__((always_inline, pure)) bool webdriverSupportedMethods(const int method) {
  return method >= GET && method <= DELETE;
}

extern __inline__ __attribute__((always_inline)) bool webdriverError(const void *pno) {

  return (const uintptr_t)pno > MIN_NERROR && (const uintptr_t)pno < MAX_NERROR;
}

extern __inline__ __attribute__((always_inline)) void webdriverPerror(const void *perr) {
  // Unimplemented
}

//HTTP CLIENT
#endif
