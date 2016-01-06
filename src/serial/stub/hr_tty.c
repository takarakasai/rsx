
#include "serial/hr_tty.h"

#include "dp_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

errno_t hr_tty_init (hr_tty *tty) {
  EVALUE(NULL, tty);

  return EOK;
}

errno_t hrtty_setup (
    hr_tty *tty, hr_unixio *io, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

  return EOK;
}

errno_t hrtty_teardown (hr_tty *tty, hr_unixio *io) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

  return EOK;
}

