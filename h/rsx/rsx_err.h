
#ifndef RSX_ERR_H
#define RSX_ERR_H

/* for printf */
#include <stdio.h>

#include "rsx_type.h"

#define EVALUE(value, variable) \
  if ((value) == (variable)) {      \
    fprintf(stderr, "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);        \
    return EINVAL;              \
  }

#define ELTGT(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue < variable)) {  \
    return EINVAL;                                   \
  }

#define ELTGE(lvalue, hvalue, variable)              \
  if ((variable < lvalue) || (hvalue <= variable)) { \
    return EINVAL;                                   \
  }

#define ELEGT(lvalue, hvalue, variable)              \
  if ((variable <= lvalue) || (hvalue < variable)) { \
    return EINVAL;                                   \
  }

#define ECALL(function)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {        \
      return eno;           \
    }                       \
  } while(0)

#endif

