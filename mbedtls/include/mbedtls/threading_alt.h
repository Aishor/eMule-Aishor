/* threading_alt.h - Windows platform mutex and condition variable types
 * For use with MBEDTLS_THREADING_ALT configuration
 */
#ifndef THREADING_ALT_H
#define THREADING_ALT_H

/* Ensure Winsock2 is included before Windows.h to prevent type redefinition
 * errors */
#include <windows.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mbedtls_threading_mutex_t {
  CRITICAL_SECTION cs;
  int is_valid;
} mbedtls_threading_mutex_t;

/* Define mbedtls_platform_mutex_t as alias for compatibility if needed */
typedef mbedtls_threading_mutex_t mbedtls_platform_mutex_t;

/* Define condition variable type used by eMule (not standard mbedtls 3.x but
 * used in TLSthreading) */
typedef struct mbedtls_platform_condition_variable_t {
  CONDITION_VARIABLE cv;
} mbedtls_platform_condition_variable_t;

#ifdef __cplusplus
}
#endif

#endif /* THREADING_ALT_H */
