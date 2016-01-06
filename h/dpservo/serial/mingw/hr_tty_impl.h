
#ifndef HR_TTY_IMPL_H
#define HR_TTY_IMPL_H

#include <windows.h>

typedef struct {
  DCB prev_ser;
  DCB ser;

  COMMTIMEOUTS prev_tout;
  COMMTIMEOUTS tout;
} hr_tty;

#endif

