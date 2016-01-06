
#ifndef HR_UNIXIO_IMPL_H
#define HR_UNIXIO_IMPL_H

#include "dp_type.h"

/* for HANDLE */
#include <windows.h>

typedef struct {
  HANDLE hd;
} hr_unixio;

#endif

