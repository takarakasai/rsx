
#ifndef DP_TYPE_H
#define DP_TYPE_H

#include <errno.h>

/* for *int*_t */
#include <stdint.h>

/* for size_t */
#include <stddef.h>

/* for bool */
#include <stdbool.h>

typedef float  float32_t;
typedef double float64_t;

#define EOK 0

typedef int errno_t;
typedef char char8_t;

static const errno_t eok = EOK;

typedef int32_t dps_opt_t;

#endif

