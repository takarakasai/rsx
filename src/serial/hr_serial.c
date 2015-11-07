
#include "serial/hr_serial.h"
#include "rsx_err.h"

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

static errno_t setraw(struct termios *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term->c_cflag &= ~(CSIZE | PARENB);
  //term->c_cflag |= CS8 | CRTSCTS;
  term->c_cflag |= CS8;

  return EOK;
}

static errno_t setspeed (struct termios *term, speed_t speed) {
  EVALUE(NULL, term);

  errno_t eno;

  eno = cfsetispeed(term, speed);
  if (eno != 0) {
    printf("file can not set input speed : %d\n", errno);
    return errno;
  }

  eno = cfsetospeed(term, speed);
  if (eno != 0) {
    printf("file can not set output speed : %d\n", errno);
    return errno;
  }

  return EOK;
}

static errno_t setattr (int fd, struct termios *term) {
  EVALUE(0, fd);
  EVALUE(NULL, term);

  int eno;
  eno = tcflush(fd, TCIFLUSH);
  if (eno != 0) {
    printf("file can not tcflush : %d\n", errno);
    return errno;
  }

  eno = tcsetattr(fd, TCSANOW, term);
  if (eno != 0) {
    printf("file can not tcsetattr : %d\n", errno);
    return errno;
  }

  return EOK;
}

errno_t hr_serial_init (hr_serial *ser) {
  EVALUE(NULL, ser);

  ser->fd = 0;
  memset(&(ser->prev_term), 0, sizeof(struct termios));
  memset(&(ser->term), 0, sizeof(struct termios));

  return EOK;
}

errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit) {
  EVALUE(NULL, ser);
  EVALUE(NULL, dev);
  EVALUE(NULL, unit);

#ifdef __unix
  const char *header = "/dev/";
#endif
  size_t len_header = strlen(header);
  size_t len_dev  = strlen(dev);
  size_t len_unit = strlen(unit);

  char path[len_header + len_dev + len_unit + 1];
  memcpy(path                       , header, len_header);
  memcpy(path + len_header          , dev   , len_dev);
  memcpy(path + len_header + len_dev, unit  , len_unit);
  path[len_header + len_dev + len_unit] = '\0';

  ECALL(_open(path, &(ser->fd)));

  tcgetattr(ser->fd, &(ser->prev_term)); /* 現在のポート設定を待避 */
  ECALL(setraw(&(ser->term)));
  ECALL(setspeed(&(ser->term), B115200));
  ECALL(setattr(ser->fd, &(ser->term)));

  return EOK;
}

errno_t hr_serial_close (hr_serial *ser) {
  EVALUE(NULL, ser);

  ECALL(setattr(ser->fd, &(ser->prev_term)));
  ser->fd = 0;

  ECALL(_close(ser->fd));

  return EOK;
}

errno_t hr_serial_read (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);

  size_t read_size;
  ECALL(_read(ser->fd, data, size, &read_size));
  printf("=== %zd %zd\n", size, read_size);

  if (size != read_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, read_size, size);
    for (size_t i = 0; i < read_size; i++) {
      printf(" %02x", ((uint8_t*)data)[i]);
    }
    printf("\n");
    return -1;
  }

  return EOK;
}

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size) {

  size_t send_size;
  ECALL(_write(ser->fd, data, size, &send_size));

  if (size != send_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, size, send_size);
    return -1;
  }

#if defined(HR_SERIAL_AUTO_READ_ECHO_DATA)
  usleep(5 * 1000);
  size_t read_size;
  ECALL(_read(ser->fd, data, size, &read_size));
#endif

  return EOK;
}

