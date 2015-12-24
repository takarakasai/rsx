#include "time/hr_unixtime.h"

#include "dp_err.h"

typedef struct timespec hr_time;

errno_t hr_get_time (hr_time *tm) {
  EVALUE(NULL, tm);
  
  int eno = clock_gettime(CLOCK_REALTIME, tm);
  
  return eno == 0 ? EOK : errno;
}

errno_t hr_diff_time (const hr_time *tm_bef, const hr_time *tm_aft, hr_time *tm_diff) {
  EVALUE(NULL, tm_bef);
  EVALUE(NULL, tm_aft);
  EVALUE(NULL, tm_diff);

  if (tm_aft->tv_nsec > tm_bef->tv_nsec) {
    tm_diff->tv_nsec = tm_aft->tv_nsec - tm_bef->tv_nsec;
    tm_diff->tv_sec  = tm_aft->tv_sec  - tm_bef->tv_sec;
  } else {
    tm_diff->tv_nsec = tm_aft->tv_nsec - tm_bef->tv_nsec + (1000 * 1000 * 1000);
    tm_diff->tv_sec  = tm_aft->tv_sec  - tm_bef->tv_sec - 1;
  }

  return EOK;
}

errno_t hr_dump_time (const hr_time *tm) {
  EVALUE(NULL, tm);

  printf("%09zd%06zd.%03zd[usec]\n", tm->tv_sec, tm->tv_nsec / 1000, tm->tv_nsec % 1000);

  return EOK;
}

