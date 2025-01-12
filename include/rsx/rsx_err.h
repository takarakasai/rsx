
#ifndef RSX_ERR_H
#define RSX_ERR_H

/* for printf */
#include <stdio.h>

#include <rsx/rsx_type.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EVALUE(value, variable) \
  if ((value) == (variable)) {      \
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
      printf("error %s:%d eno:%d\n", __func__, __LINE__, eno);  \
      return eno;           \
    }                       \
  } while(0)

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif

