
#ifndef RSX_ERR_H
#define RSX_ERR_H

/* for printf */
#include <stdio.h>

#include "rsx_type.h"

#define EVALUE_ERRNO(value, variable, errno) \
  if ((value) == (variable)) {      \
    fprintf(stderr, "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);        \
    return errno;              \
  }

#define EVALUE(value, variable) EVALUE_ERRNO(value, variable, EINVAL)

#define ELTGT(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue < variable)) {  \
    fprintf(stderr, "--> %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); \
    return EINVAL;                                   \
  }

#define ELTGE(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue <= variable)) { \
    fprintf(stderr, "--> %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); \
    return EINVAL;                                   \
  }

#define ELEGT(lvalue, hvalue, variable)              \
  if ((variable <= lvalue) || (hvalue < variable)) { \
    fprintf(stderr, "--> %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); \
    return EINVAL;                                   \
  }

#define ECALL(function)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {        \
      fprintf(stderr, "--> %s %s %d eno:%d\n", __FILE__, __FUNCTION__, __LINE__, eno); \
      return eno;           \
    }                       \
  } while(0)

#endif

