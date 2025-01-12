#include "rsx/util/time.h"

#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  #error environment not supported
#else
  #include <time.h>
  #include <unistd.h>
  #include <sys/select.h>
#endif

errno_t rsx_pause() {
#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
  pause();
#else
#endif
  return EOK;
}

errno_t rsx_wait_usec(long usec) {
#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#else
  struct timespec time;
  time.tv_sec  = (usec / (1000 * 1000));
  time.tv_nsec = (usec % (1000 * 1000)) * 1000;
  pselect(0, 0, 0, 0, &time, 0);
#endif
  return EOK;
}

errno_t rsx_gettime(rsx_timespec* timespec) {
  EVALUE(NULL, timespec);
#if defined(CYGWIN) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#else
  struct timespec time;
  int res = clock_gettime(CLOCK_REALTIME, &time);
  if (res == -1) {
    return errno;
  }

  timespec->sec  = time.tv_sec;
  timespec->nsec = time.tv_nsec;

  return EOK;
#endif
}

errno_t rsx_difftime(rsx_timespec* after, rsx_timespec* before, rsx_timespec* result) {
  EVALUE(NULL, after);
  EVALUE(NULL, before);
  EVALUE(NULL, result);

  if (after->sec < before->sec) {
    return EINVAL;
  }
  if (after->sec == before->sec && after->nsec < before->nsec) {
    return EINVAL;
  }

  if (after->nsec < before->nsec) {
    result->sec  = after->sec - before->sec - 1;
    result->nsec = after->nsec + (1000 * 1000 * 1000 - before->nsec);
  } else {
    result->sec  = after->sec - before->sec;
    result->nsec = after->nsec - before->nsec;
  }

  return EOK;
}
