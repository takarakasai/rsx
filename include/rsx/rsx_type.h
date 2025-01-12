
#ifndef RSX_TYPE_H
#define RSX_TYPE_H

#include <errno.h>

/* for *int*_t */
#include <stdint.h>

/* for size_t */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EOK 0

typedef int errno_t;

static const errno_t eok = EOK;

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif

