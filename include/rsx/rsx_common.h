#ifndef RSX_COMMON_H_
#define RSX_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(RSX_DEBUG_PRINT_ENABLE)
  #define RSX_DEBUG_PRINT(...) fprintf(stdout, __VA_ARGS__)
  #define RSX_DEBUG_EPRINT(...) fprintf(stderr, __VA_ARGS__)
#else
  #define RSX_DEBUG_PRINT(...)
  #define RSX_DEBUG_EPRINT(...)
#endif

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif  // RSX_COMMON_H_
