
#include "serial/hr_serial.h"

#include "rsx_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

/* for usleep */
#include <unistd.h>

static const size_t timeout_usec = 50 * 1000;
static const size_t wait_usec    = 10;

static errno_t setraw(struct termios *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  //term->c_cflag &= ~(CSIZE | PARENB);
  term->c_cflag &= ~(CSIZE | PARENB | CRTSCTS);
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

static errno_t serial_read (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);
  EVALUE(NULL, data);

  errno_t eno = -1; // TODO:

  const size_t max_count = (timeout_usec + wait_usec - 1) / wait_usec;
  size_t rcv_cnt = 0;
  size_t cnt;
  for (cnt = 0; cnt < max_count; cnt++) {
    usleep(wait_usec);

    size_t read_size;
    uint8_t rdata[size];
    ECALL(_read(ser->fd, rdata, size, &read_size));

    // overflow TODO:
    for (size_t i = 0; i < read_size; i++) {
      ((uint8_t*)data)[rcv_cnt + i] = rdata[i];
    }
    rcv_cnt += read_size;

    if (size == rcv_cnt) {
      eno = EOK;
      break;
    }
  }

  //printf("           read latency : %zd / %zd %s\n",
  //        cnt * wait_usec, max_count * wait_usec, size == rcv_cnt ? "OK" : "NG");

#if DEBUG
  printf("           size[%03zd] :", rcv_cnt);
  for (size_t i = 0; i < rcv_cnt; i++) {
    printf(" %02x", ((uint8_t*)data)[i]);
  }
  printf("\n");
#endif

  return eno;
}

errno_t hr_serial_read (hr_serial *ser, void* data, size_t size) {

#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

  ECALL(serial_read(ser, data, size));

#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf("           read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif

  return EOK;
}

errno_t hr_serial_write (hr_serial *ser, void* data, size_t size) {
  EVALUE(NULL, ser);
  EVALUE(NULL, data);

  size_t send_size;
  ECALL(_write(ser->fd, data, size, &send_size));

  if (size != send_size) {
    printf("error %s %zd / %zd\n", __FUNCTION__, size, send_size);
    return -1;
  }

#if defined(HR_SERIAL_AUTO_READ_ECHO_DATA)
#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  hr_time tm_bef, tm_aft, tm_diff;
  ECALL(hr_get_time(&tm_bef));
#endif

  const size_t max_count = (timeout_usec + wait_usec - 1) / wait_usec;
  size_t cnt;
  for (cnt = 0; cnt < max_count; cnt++) {
    usleep(wait_usec);
    size_t recv_size;
    uint8_t rdata[send_size];
    ECALL(_read(ser->fd, rdata, send_size, &recv_size));

    if (recv_size != send_size) {
      continue;
    }

    if (strcmp((void*)rdata, data) == 0) {
      break;
    }
  }
#if defined(HR_SERIAL_LATENCY_CHECK_DETAIL)
  //printf(" auto echo read latency : %zd / %zd\n", cnt * wait_usec, max_count * wait_usec);
  ECALL(hr_get_time(&tm_aft));
  ECALL(hr_diff_time(&tm_bef, &tm_aft, &tm_diff));

  printf(" auto echo read latency : ");
  ECALL(hr_dump_time(&tm_diff));
#endif
#endif

  return EOK;
}

