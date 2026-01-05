#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_threading_mutex_t
{
    CRITICAL_SECTION cs;
    int is_valid;
} mbedtls_threading_mutex_t;

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT_H */
