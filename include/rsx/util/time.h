#ifndef RSX_UTIL_TIME_H_
#define RSX_UTIL_TIME_H_

#ifdef __cplusplus
extern "C" {
#endif

void rsx_pause();

void rsx_wait_usec(long usec);

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif  // RSX_UTIL_TIME_H_
