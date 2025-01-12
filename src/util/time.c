#include <rsx/util/time.h>

#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #error environment not supported
#else
  #include <unistd.h>
  #include <sys/select.h>
#endif

void rsx_pause() {
#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  pause();
#else
#endif
}

void rsx_wait_usec(long usec) {
#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#else
  struct timespec time;
  time.tv_sec  = (usec / (1000 * 1000));
  time.tv_nsec = (usec % (1000 * 1000)) * 1000;
  pselect(0, 0, 0, 0, &time, 0);
#endif
}
