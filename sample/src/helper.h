
#ifndef HELPER_H
#define HELPER_H

/* for printf */
#include <stdio.h>

/* for strcmp */
#include <string.h>

#include "dpservo.h"

static inline errno_t dps_setup_rsx (dpservo_base *base, int argc, char *argv[], int *argc_offset) {
  EVALUE(NULL, base);
  EVALUE(NULL, argc_offset);

  bool use_serial = true;

  hr_baudrate baudrate = HR_B115200;

  char *device = "ttyUSB";
  char *port   = "0";

  if (argc >= 4) {
    device = argv[1];
    port   = argv[2];

    if (strcmp(argv[3], "high") == 0) {
      baudrate =  HR_B460800;
    } else {
      baudrate =  HR_B115200;
    }

    *argc_offset = 4;
  } else {
    return EINVAL;
  }

  ECALL(dps_set_serial(base, use_serial));

  printf("===: %s/%s with %s(%s)\n", device, port, hr_baudrate2str(baudrate), argv[3]);
  ECALL(dps_open(base, device, port, baudrate, HR_PAR_EVEN));

  return EOK;
}

static inline errno_t dps_setup (dpservo_base *base, int argc, char *argv[], int *argc_offset) {
  EVALUE(NULL, base);
  EVALUE(NULL, argc_offset);

  bool use_serial = true;

  hr_baudrate baudrate = HR_B115200;

  char *device = "ttyUSB";
  char *port   = "0";

  if (argc >= 4) {
    device = argv[1];
    port   = argv[2];

    if (strcmp(argv[3], "high") == 0) {
      baudrate = HR_B1250000;
    } else if(strcmp(argv[3], "mid") == 0) {
      baudrate =  HR_B625000;
    } else {
      baudrate =  HR_B115200;
    }

    *argc_offset = 4;
  } else {
    return EINVAL;
  }

  ECALL(dps_set_serial(base, use_serial));

  printf("===: %s/%s with %s(%s)\n", device, port, hr_baudrate2str(baudrate), argv[3]);
  ECALL(dps_open(base, device, port, baudrate, HR_PAR_EVEN));

  return EOK;
}

static inline errno_t dps_teardown (dpservo_base *base, int argc, char *argv[]) {
  EVALUE(NULL, base);

  ECALL(dps_close(base));

  return EOK;
}

#endif

