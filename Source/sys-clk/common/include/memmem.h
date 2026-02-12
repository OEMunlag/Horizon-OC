#ifndef MEMMEM_IMPL_H
#define MEMMEM_IMPL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memmem_impl(void *haystack, size_t haystacklen,
                  const void *needle, size_t needlelen);

#ifdef __cplusplus
}
#endif

#endif
