
#ifndef HR_SERIAL_H
#define HR_SERIAL_H

#define TERMIOS2

#if defined(TERMIOS2)
#include <asm/termios.h>
#else
#include <termios.h>
#endif

#include "dp_err.h"
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

#if 0
static inline errno_t hr_str2parity (const char* cparity, hr_parity *parity) {
  EVALUE(NULL, cparity);
  EVALUE(NULL, parity);

  if (strcmp(cparity, "none") == 0) {
    *parity = HR_PAR_NONE;
  } else if (strcmp(cparity, "even")) {
    *parity = HR_PAR_EVEN;
  } else if (strcmp(cparity, "odd")) {
    *parity = HR_PAR_ODD;
  } else {
    return EINVAL;
  }

  return EOK;
}
#endif

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

static inline errno_t hr_int2baudrate (int32_t baudrate, hr_baudrate *pbaudrate) {
  EVALUE(NULL, pbaudrate);

  switch (baudrate) {
    case 9600     : *pbaudrate = HR_B9600   ; break;
    case 19200    : *pbaudrate = HR_B19200  ; break;
    case 38400    : *pbaudrate = HR_B38400  ; break;
    case 57600    : *pbaudrate = HR_B57600  ; break;
    case 115200   : *pbaudrate = HR_B115200 ; break;
    case 230400   : *pbaudrate = HR_B230400 ; break;
    case 460800   : *pbaudrate = HR_B460800 ; break;
    case 576000   : *pbaudrate = HR_B576000 ; break;
    case 625000   : *pbaudrate = HR_B625000 ; break;
    case 1152000  : *pbaudrate = HR_B1152000; break;
    case 1250000  : *pbaudrate = HR_B1250000; break;
    default       : return EINVAL ;
  }

  return EOK;
}

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

#if defined(TERMIOS2)
typedef struct termios2 TERMIOS;
#else
typedef struct termios  TERMIOS;
#endif

typedef struct {
  int fd;

  //hr_parity   parity;
  hr_baudrate baudrate;

  TERMIOS prev_term;
  TERMIOS term;
} hr_serial;

errno_t hr_serial_init (hr_serial *ser);
errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit, hr_baudrate baudrate, hr_parity parity);
errno_t hr_serial_close (hr_serial *ser);
errno_t hr_serial_write (hr_serial *ser, void* data, size_t size);
errno_t hr_serial_read (hr_serial *ser, void* data, size_t size);

#endif

