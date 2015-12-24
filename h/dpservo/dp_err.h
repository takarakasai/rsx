
#ifndef DP_ERR_H
#define DP_ERR_H

/* for printf */
#include <stdio.h>

#include "dp_type.h"

#define EXPECT_VALUE_ERRNO(value, variable, errno) \
  if ((value) != (variable)) {      \
    fprintf(stderr, "%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);        \
    return errno;              \
  }

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

#define ELEGE(lvalue, hvalue, variable)              \
  if ((variable <= lvalue) || (hvalue <= variable)) { \
    fprintf(stderr, "--> %s %s %d\n", __FILE__, __FUNCTION__, __LINE__); \
    return EINVAL;                                   \
  }

#define ECALL2(function, print_flg)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {       \
        if (print_flg) {    \
          fprintf(stderr, "--> %s %s %d eno:%d\n", __FILE__, __FUNCTION__, __LINE__, eno); \
        }                   \
      return eno;           \
    }                       \
  } while(0)

#define ECALL_THROW2(function, print_flg)     \
  do {                      \
    errno_t eno = function; \
    if (eno != EOK) {       \
        if (print_flg) {    \
          fprintf(stderr, "--> %s %s %d eno:%d\n", __FILE__, __FUNCTION__, __LINE__, eno); \
        }                   \
      throw eno;            \
    }                       \
  } while(0)

#define ECALL(function) ECALL2(function, true)
#define ECALL_THROW(function) ECALL_THROW2(function, true)

#endif

