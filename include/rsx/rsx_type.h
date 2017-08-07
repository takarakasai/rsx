
#ifndef RSX_TYPE_H
#define RSX_TYPE_H

#include <errno.h>

/* for *int*_t */
#include <stdint.h>

/* for size_t */
#include <stddef.h>

#define EOK 0

typedef int errno_t;

static const errno_t eok = EOK;

#endif

