
#ifndef HR_TTY_IMPL_H
#define HR_TTY_IMPL_H

#define TERMIOS2

#if defined(TERMIOS2)
#include <asm/termios.h>
#else
#include <termios.h>
#endif

#if defined(TERMIOS2)
typedef struct termios2 TERMIOS;
#else
typedef struct termios  TERMIOS;
#endif

typedef struct {
  TERMIOS prev_term;
  TERMIOS term;
} hr_tty;

#endif

