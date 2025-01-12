#ifndef RSX_UTIL_TIME_H_
#define RSX_UTIL_TIME_H_

#include "rsx/rsx_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  long sec;
  long nsec;
} rsx_timespec;

errno_t rsx_pause();

errno_t rsx_wait_usec(long usec);

errno_t rsx_gettime(rsx_timespec* timespec);

errno_t rsx_difftime(rsx_timespec* after, rsx_timespec* before, rsx_timespec* result);

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif  // RSX_UTIL_TIME_H_
