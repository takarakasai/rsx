
#ifndef HR_SERIAL_H
#define HR_SERIAL_H

#include <termios.h>

#include "hr_unixio.h"

#define HR_SERIAL_DECL(name) \
  hr_serial name

#define HR_SERIAL_INIT(name) \
  hr_serial_init(&name)

typedef enum {
  HR_PAR_NONE,
  HR_PAR_EVEN,
  HR_PAR_ODD,
} hr_parity;

typedef enum {
  HR_B9600   ,
  HR_B19200  ,
  HR_B38400  ,
  HR_B57600  ,
  HR_B115200 ,
  HR_B230400 ,
  HR_B460800 ,
  HR_B576000 ,
  HR_B1152000
} hr_baudrate;

typedef struct {
  int fd;

  //hr_parity   parity;
  hr_baudrate baudrate;

  struct termios prev_term;
  struct termios term;
} hr_serial;

errno_t hr_serial_init (hr_serial *ser);
errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit, hr_baudrate baudrate, hr_parity parity);
errno_t hr_serial_close (hr_serial *ser);
errno_t hr_serial_write (hr_serial *ser, void* data, size_t size);
errno_t hr_serial_read (hr_serial *ser, void* data, size_t size);

#endif

