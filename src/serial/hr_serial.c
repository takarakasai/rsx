
#include "serial/hr_serial.h"

#include "dp_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

/* for usleep */
#include <unistd.h>

#define ICS

static const size_t timeout_usec = 200; // TODO: ICS --> 100 | RSX --> 200
static const size_t wait_usec    = 10;

static errno_t setraw(struct termios *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term->c_cflag &= ~(CSIZE | CRTSCTS);          /* flag off */
  term->c_cflag |= CS8;

  /// TODO:AAA
  //term->c_cflag &= ~CBAUD; // ~0|010017 --> ~0b 0 001 000 000 001 111 --> 0b 1 110 111 111 110 000
  //term->c_cflag |= BOTHER; //  0|010000 -->  0b 0 001 000 000 000 000 // CBAUDEX?
  //term->c_ispeed = 1250000;
  //term->c_ospeed = 1250000;

  return EOK;
}

static errno_t set_cflag (struct termios *term, tcflag_t parity) {
  EVALUE(NULL, term);

  term->c_cflag |= parity;                      /* flag on  */

  return EOK;
}

static errno_t unset_cflag (struct termios *term, tcflag_t parity) {
  EVALUE(NULL, term);

  term->c_cflag &= ~parity;                     /* flag off  */

  return EOK;
}

static errno_t set_parity (struct termios *term, hr_parity parity) {
  EVALUE(NULL, term);

  switch(parity) {
      case HR_PAR_NONE:
      ECALL(unset_cflag(term, PARENB));
      break;
    case HR_PAR_EVEN:
      ECALL(set_cflag(term, PARENB));
      ECALL(unset_cflag(term, PARODD));
      break;
    case HR_PAR_ODD:
      ECALL(set_cflag(term, PARENB));
      ECALL(set_cflag(term, PARODD));
      break;
    default:
      return EINVAL;
      break;
  }

  return EOK;
}

static errno_t _setspeed (struct termios *term, hr_baudrate baudrate) {
  EVALUE(NULL, term);

  speed_t speed;
  switch (baudrate) {
    case HR_B9600   : speed = B9600    ; break;
    case HR_B19200  : speed = B19200   ; break;
    case HR_B38400  : speed = B38400   ; break;
    case HR_B57600  : speed = B57600   ; break;
    case HR_B115200 : speed = B115200  ; break;
    case HR_B230400 : speed = B230400  ; break;
    case HR_B460800 : speed = B460800  ; break;
    case HR_B576000 : speed = B576000  ; break;
    case HR_B625000 : return EINVAL;
    case HR_B1152000: speed = B1152000 ; break;
    case HR_B1250000: return EINVAL;
    default :
      return EINVAL;
  }

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

static errno_t _setspeed2 (struct termios *term, int speed) {
  EVALUE(NULL, term);

#if 0
  int speed;
  switch (baudrate) {
    case HR_B9600   : speed = 9600    ; break;
    case HR_B19200  : speed = 19200   ; break;
    case HR_B38400  : speed = 38400   ; break;
    case HR_B57600  : speed = 57600   ; break;
    case HR_B115200 : speed = 115200  ; break;
    case HR_B230400 : speed = 230400  ; break;
    case HR_B460800 : speed = 460800  ; break;
    case HR_B576000 : speed = 576000  ; break;
    case HR_B1152000: speed = 1152000 ; break;
    case HR_B1250000: speed = 1250000 ; break;
    default :
      return EINVAL;
  }
#endif

  term->c_cflag &= ~CBAUD;   // ~0|010017 --> ~0b 0 001 000 000 001 111 --> 0b 1 110 111 111 110 000
  //term->c_cflag |= BOTHER; //  0|010000 -->  0b 0 001 000 000 000 000 // CBAUDEX?
  term->c_cflag |= CBAUDEX;  //  0|010000 -->  0b 0 001 000 000 000 000 // CBAUDEX?
 
  term->c_ispeed = speed;

  term->c_ospeed = speed;

  return EOK;
}

static errno_t setspeed (struct termios *term, hr_baudrate baudrate) {
  EVALUE(NULL, term);

  switch (baudrate) {
    case HR_B9600   :
    case HR_B19200  :
    case HR_B38400  :
    case HR_B57600  :
    case HR_B115200 :
    case HR_B230400 :
    case HR_B460800 :
    case HR_B576000 :
      ECALL(_setspeed(term, baudrate)); break;
    case HR_B625000 :
      ECALL(_setspeed2(term,  625000)); break;
    case HR_B1152000:
      ECALL(_setspeed(term, baudrate)); break;
    case HR_B1250000:
      ECALL(_setspeed2(term, 1250000)); break;
    default :
      return EINVAL;
  }

  return EOK;
}


static errno_t setattr (int fd, struct termios *term) {
  EVALUE(0, fd);
  EVALUE(NULL, term);

  int eno;

  // TODO:AAA
  eno = tcflush(fd, TCIFLUSH);
  if (eno != 0) {
    printf("file can not tcflush : %d\n", errno);
    return errno;
  }

  // TODO:AAA
  eno = tcsetattr(fd, TCSANOW, term);
  //eno = ioctl(fd, TCSETS2, term);
  if (eno != 0) {
    printf("file can not tcsetattr : %d\n", errno);
    return errno;
  }

  return EOK;
}

errno_t hr_serial_init (hr_serial *ser) {
  EVALUE(NULL, ser);

  ser->fd = 0;

  ser->baudrate = HR_B115200;

  memset(&(ser->prev_term), 0, sizeof(struct termios));
  memset(&(ser->term), 0, sizeof(struct termios));

  return EOK;
}

errno_t hr_serial_open (hr_serial *ser, const char* dev, const char* unit, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, ser);
  EVALUE(NULL, dev);
  EVALUE(NULL, unit);

  ser->baudrate = baudrate;

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

  //TODO:AAA
  tcgetattr(ser->fd, &(ser->prev_term)); /* 現在のポート設定を待避 */
  //ioctl(ser->fd, TCGETS2, &(ser->prev_term));

  ECALL(setraw(&(ser->term)));

  ECALL(set_parity(&(ser->term), parity));

  ECALL(setspeed(&(ser->term), baudrate));

  ECALL(setattr(ser->fd, &(ser->term)));

  return EOK;
}

errno_t hr_serial_close (hr_serial *ser) {
  EVALUE(NULL, ser);

  if (ser->fd == 0) {
    return EOK;
  }

  ECALL(setattr(ser->fd, &(ser->prev_term)));
  ECALL(_close(ser->fd));
  ser->fd = 0;

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

  //printf("           read latency : %zd / %zd %s (%zd/%zd)\n",
  //        cnt * wait_usec, max_count * wait_usec, size == rcv_cnt ? "OK" : "NG", rcv_cnt, size);

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

  ECALL2(serial_read(ser, data, size), false);

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
    printf("error %s %zd / %zd\n", __FUNCTION__, send_size, size);
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

    if (memcmp(rdata, data, recv_size) == 0) {
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

