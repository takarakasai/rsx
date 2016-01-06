
#include "serial/hr_tty.h"

#include "dp_err.h"
#include "time/hr_unixtime.h"

/* for error print */
#include <stdio.h>

errno_t hr_tty_init (hr_tty *tty) {
  EVALUE(NULL, tty);

  memset(&(tty->prev_ser), 0, sizeof(DCB));
  memset(&(tty->ser), 0, sizeof(DCB));

  memset(&(tty->prev_tout), 0, sizeof(COMMTIMEOUTS));
  memset(&(tty->tout), 0, sizeof(COMMTIMEOUTS));

  return EOK;
}

static errno_t baudrate2dword (hr_baudrate baudrate, DWORD *BaudRate) {
  EVALUE(NULL, BaudRate);

  switch (baudrate) {
    case HR_B9600   : *BaudRate =    9600; break;
    case HR_B19200  : *BaudRate =   19200; break;
    case HR_B38400  : *BaudRate =   38400; break;
    case HR_B57600  : *BaudRate =   57600; break;
    case HR_B115200 : *BaudRate =  115200; break;
    case HR_B230400 : *BaudRate =  230400; break;
    case HR_B460800 : *BaudRate =  460800; break;
    case HR_B576000 : *BaudRate =  576000; break;
    case HR_B625000 : *BaudRate =  625000; break;
    case HR_B1152000: *BaudRate = 1152000; break;
    case HR_B1250000: *BaudRate = 1250000; break;
    default :
      return EINVAL;
  }

  return EOK;
}

static errno_t parity2dword (hr_parity parity, DWORD *Parity) {
  EVALUE(NULL, Parity);

  switch(parity) {
    case HR_PAR_NONE: *Parity =   NOPARITY; break;
    case HR_PAR_EVEN: *Parity = EVENPARITY; break;
    case HR_PAR_ODD:  *Parity =  ODDPARITY; break;
    default:
      return EINVAL;
      break;
  }

  return EOK;
}

errno_t hrtty_setup (
    hr_tty *tty, hr_unixio *io, hr_baudrate baudrate, hr_parity parity) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

  tty->prev_ser.DCBlength=sizeof(DCB);
  if (!GetCommState(io->hd, &(tty->prev_ser))) {
    //could not get the state of the comport
  }
  memcpy(&(tty->ser), &(tty->prev_ser), sizeof(DCB));

  DWORD BaudRate;
  ECALL(baudrate2dword(baudrate, &BaudRate));
  tty->ser.BaudRate = BaudRate;

  tty->ser.ByteSize = 8;
  tty->ser.StopBits = ONESTOPBIT;

  DWORD Parity;
  ECALL(parity2dword(parity, &Parity));
  tty->ser.Parity = Parity;

  if (!SetCommState(io->hd, &(tty->ser))) {
    //analyse error
  }

  //////////////////////////////////////////////////////////

  if (!GetCommTimeouts(io->hd, &(tty->prev_tout))) {
  }
  memcpy(&(tty->tout), &(tty->prev_tout), sizeof(COMMTIMEOUTS));

  tty->tout.ReadIntervalTimeout = 0;
  tty->tout.ReadTotalTimeoutMultiplier  =   0;
  tty->tout.ReadTotalTimeoutConstant    =  10;
  tty->tout.WriteTotalTimeoutMultiplier =   0;
  tty->tout.WriteTotalTimeoutConstant   =  10;

  if (!SetCommTimeouts(io->hd, &(tty->tout))) {
  }
 
  return EOK;
}

errno_t hrtty_teardown (hr_tty *tty, hr_unixio *io) {
  EVALUE(NULL, tty);
  EVALUE(NULL, io);

  if(!SetCommState(io->hd, &(tty->prev_ser))){
    //analyse error
  }

  if (!SetCommTimeouts(io->hd, &(tty->prev_tout))) {
  }

  return EOK;
}

