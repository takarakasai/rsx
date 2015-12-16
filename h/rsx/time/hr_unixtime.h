
#include <time.h>

#include "dp_type.h"
//#include "rsx_type.h"

typedef struct timespec hr_time;

errno_t hr_get_time (hr_time *tm);

errno_t hr_diff_time (const hr_time *tm_bef, const hr_time *tm_aft, hr_time *tm_diff);
 
errno_t hr_dump_time (const hr_time *tm);

