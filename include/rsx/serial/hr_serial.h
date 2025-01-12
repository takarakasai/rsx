
#ifndef HR_SERIAL_H
#define HR_SERIAL_H

#include <termios.h>

#include <rsx/serial/hr_unixio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int fd;
  speed_t baudrate;
  struct termios prev_term;
  struct termios term;

  size_t offset;
  uint8_t buff[1024];
} hr_serial;

errno_t hr_serial_init (hr_serial *ser);
errno_t hr_serial_set_baudrate(hr_serial *ser, int baudrate);

errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit);
errno_t hr_serial_close (hr_serial *ser);

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size);
errno_t hr_serial_read (hr_serial *ser, void* data, size_t size);

#ifdef __cplusplus
}  //  extern "C"
#endif

#endif

