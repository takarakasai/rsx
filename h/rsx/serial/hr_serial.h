
#ifndef HR_SERIAL_H
#define HR_SERIAL_H

#include <termios.h>

#include "hr_unixio.h"

typedef struct {
  int fd;
  struct termios prev_term;
  struct termios term;
} hr_serial;

errno_t hr_serial_init (hr_serial *ser);
errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit);
errno_t hr_serial_close (hr_serial *ser);
errno_t hr_serial_write (hr_serial *ser, void* data, size_t size);
errno_t hr_serial_read (hr_serial *ser, void* data, size_t size);

#endif

