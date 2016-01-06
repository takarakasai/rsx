
#include "serial/hr_tty.h"

#include "dp_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

/* for memset */
#include <string.h>

/* for usleep */
#include <unistd.h>

#if defined(TERMIOS2)
/* we can not include sys/ioctl.h. because struct winsize is duplicated
 * in termios.h and ioctl.h */
extern int ioctl (int __fd, unsigned long int __request, ...) __THROW;
extern int tcflush(int fd, int queue_selector);
#endif

static errno_t setraw(TERMIOS *term) {
  EVALUE(NULL, term);

  term->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  term->c_oflag &= ~OPOST;
  term->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  term->c_cflag &= ~(CSIZE | CRTSCTS);          /* flag off */
  term->c_cflag |= CS8;

  return EOK;
}

static errno_t set_cflag (TERMIOS *term, tcflag_t parity) {
  EVALUE(NULL, term);

  term->c_cflag |= parity;                      /* flag on  */

  return EOK;
}

static errno_t unset_cflag (TERMIOS *term, tcflag_t parity) {
  EVALUE(NULL, term);

  term->c_cflag &= ~parity;                     /* flag off  */

  return EOK;
}

static errno_t set_parity (TERMIOS *term, hr_parity parity) {
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

#if !defined(TERMIOS2)
static errno_t _setspeed (TERMIOS *term, hr_baudrate baudrate) {
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
#endif

#if defined(TERMIOS2)
static errno_t _setspeed2 (TERMIOS *term, int speed) {
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

  //term->c_cflag &= ~CBAUD;   // ~0|010017 --> ~0b 0 001 000 000 001 111 --> 0b 1 110 111 111 110 000
  term->c_cflag |= BOTHER; //  0|010000 -->  0b 0 001 000 000 000 000 // CBAUDEX?
  term->c_cflag |= CBAUDEX;  //  0|010000 -->  0b 0 001 000 000 000 000 // CBAUDEX?
 
  term->c_ispeed = speed;

  term->c_ospeed = speed;

  return EOK;
}

static errno_t setspeed (TERMIOS *term, hr_baudrate baudrate) {
  EVALUE(NULL, term);

  switch (baudrate) {
    case HR_B9600   :
      ECALL(_setspeed2(term,    9600)); break;
    case HR_B19200  :
      ECALL(_setspeed2(term,   19200)); break;
    case HR_B38400  :
      ECALL(_setspeed2(term,   38400)); break;
    case HR_B57600  :
      ECALL(_setspeed2(term,   57600)); break;
    case HR_B115200 :
      ECALL(_setspeed2(term,  115200)); break;
    case HR_B230400 :
      ECALL(_setspeed2(term,  230400)); break;
    case HR_B460800 :
      ECALL(_setspeed2(term,  460800)); break;
    case HR_B576000 :
      ECALL(_setspeed2(term,  576000)); break;
    case HR_B625000 :
      ECALL(_setspeed2(term,  625000)); break;
    case HR_B1152000:
      ECALL(_setspeed2(term, 1152000)); break;
    case HR_B1250000:
      ECALL(_setspeed2(term, 1250000)); break;
    default :
      return EINVAL;
  }

  return EOK;
}
#else
static errno_t setspeed (TERMIOS *term, hr_baudrate baudrate) {
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
      //ECALL(_setspeed2(term,  625000)); break;
      return EINVAL;
    case HR_B1152000:
      ECALL(_setspeed(term, baudrate)); break;
    case HR_B1250000:
      //ECALL(_setspeed2(term, 1250000)); break;
      return EINVAL;
    default :
      return EINVAL;
  }

  return EOK;
}
#endif

static errno_t setattr (int fd, TERMIOS *term) {
  EVALUE(0, fd);
  EVALUE(NULL, term);

  int eno;

  eno = tcflush(fd, TCIFLUSH);
  if (eno != 0) {
    printf("file can not tcflush : %d\n", errno);
    return errno;
  }

#if defined(TERMIOS2)
  eno = ioctl(fd, TCSETS2, term);
#else
  eno = tcsetattr(fd, TCSANOW, term);
#endif
  if (eno != 0) {
    printf("file can not tcsetattr : %d\n", errno);
    return errno;
  }

  return EOK;
}

errno_t hr_tty_init (hr_tty *tty) {
  EVALUE(NULL, tty);

  memset(&(tty->prev_term), 0, sizeof(TERMIOS));
  memset(&(tty->term), 0, sizeof(TERMIOS));

  return EOK;
}

errno_t hrtty_setup (
    hr_tty *tty, hr_unixio *io, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

#if defined(TERMIOS2)
  ioctl(io->fd, TCGETS2, &(tty->prev_term));
#else
  tcgetattr(io->fd, &(tty->prev_term)); /* 現在のポート設定を待避 */
#endif

  ECALL(setraw(&(tty->term)));

  ECALL(set_parity(&(tty->term), parity));

  ECALL(setspeed(&(tty->term), baudrate));

  ECALL(setattr(io->fd, &(tty->term)));

  return EOK;
}

errno_t hrtty_teardown (hr_tty *tty, hr_unixio *io) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

  ECALL(setattr(io->fd, &(tty->prev_term)));

  return EOK;
}

