#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

typedef struct mbedtls_threading_mutex_t {
  CRITICAL_SECTION cs;
  int is_valid;
} mbedtls_threading_mutex_t;

void threading_mutex_init_alt(mbedtls_threading_mutex_t *mutex);
void threading_mutex_free_alt(mbedtls_threading_mutex_t *mutex);
int threading_mutex_lock_alt(mbedtls_threading_mutex_t *mutex);
int threading_mutex_unlock_alt(mbedtls_threading_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT_H */
