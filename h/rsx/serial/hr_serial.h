
#ifndef HR_SERIAL_H
#define HR_SERIAL_H

#include <termios.h>
// TODO:AAA
//#include <asm/termios.h>

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
  HR_B625000 ,
  HR_B1152000,
  HR_B1250000
} hr_baudrate;

static inline const char* hr_baudrate2str (hr_baudrate baudrate) {
  switch (baudrate) {
    case HR_B9600     : return "HR_B9600   ";
    case HR_B19200    : return "HR_B19200  ";
    case HR_B38400    : return "HR_B38400  ";
    case HR_B57600    : return "HR_B57600  ";
    case HR_B115200   : return "HR_B115200 ";
    case HR_B230400   : return "HR_B230400 ";
    case HR_B460800   : return "HR_B460800 ";
    case HR_B576000   : return "HR_B576000 ";
    case HR_B625000   : return "HR_B625000 ";
    case HR_B1152000  : return "HR_B1152000";
    case HR_B1250000  : return "HR_B1250000";
    default           : return "HR_INVALID" ;
  }
}

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

