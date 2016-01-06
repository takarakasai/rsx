#include "time/hr_unixtime.h"

#include "dp_err.h"

// for usleep
#include <unistd.h>

typedef struct timespec hr_time;

errno_t hr_get_time (hr_time *tm) {
  EVALUE(NULL, tm);

#if defined(__unix)
  int eno = clock_gettime(CLOCK_REALTIME, tm);
  return eno == 0 ? EOK : errno;
#else
  return EOK;
#endif
}

errno_t hr_diff_time (const hr_time *tm_bef, const hr_time *tm_aft, hr_time *tm_diff) {
  EVALUE(NULL, tm_bef);
  EVALUE(NULL, tm_aft);
  EVALUE(NULL, tm_diff);

#if defined(__unix)
  if (tm_aft->tv_nsec > tm_bef->tv_nsec) {
    tm_diff->tv_nsec = tm_aft->tv_nsec - tm_bef->tv_nsec;
    tm_diff->tv_sec  = tm_aft->tv_sec  - tm_bef->tv_sec;
  } else {
    tm_diff->tv_nsec = tm_aft->tv_nsec - tm_bef->tv_nsec + (1000 * 1000 * 1000);
    tm_diff->tv_sec  = tm_aft->tv_sec  - tm_bef->tv_sec - 1;
  }
#endif

  return EOK;
}

errno_t hr_dump_time (const hr_time *tm) {
  EVALUE(NULL, tm);

#if defined(__unix)
  printf("%09zd%06zd.%03zd[usec]\n", tm->tv_sec, tm->tv_nsec / 1000, tm->tv_nsec % 1000);
#endif

  return EOK;
}

errno_t hr_usleep (const uint32_t usec) {

  struct timespec ts;
  if (usec >= 1000 * 1000) {
    ts.tv_sec =   usec / (1000 * 1000);
    ts.tv_nsec = (usec % (1000 * 1000)) * 1000;
  } else {
    ts.tv_sec  = 0;
    ts.tv_nsec = usec * 1000;
  }

  nanosleep( &ts, NULL);

  return EOK;
}

