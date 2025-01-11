/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx.h"

#include <stdbool.h>

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs, uint8_t id) {
  EVALUE(NULL, hrs);

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  errno_t eno = rsx_check_connection(&rsx, hrs, id);

  if (eno == EOK) {
    printf("OK\n");
  } else {
    printf("NG\n");
  }

  // ECALL(rsx_reboot(&rsx, hrs, id));
  // ECALL(dump(&rsx));

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  
  uint8_t id = 0x01;
  char* dev  = "ttyUSB";
  char* unit = "1";
  if (argc >= 3) {
    dev  = argv[1];
    unit = argv[2];
  }

  int baudrates[] = {
    9600,
    14400,
    19200,
    28800,
    38400,
    57600,
    115200,
    153600,
    230400
  };
  for (int i = 0; i < sizeof(baudrates)/sizeof(baudrates[0]); i++) {
    printf("try to connect : baudrate:%d\n", baudrates[i]);
    errno_t eno = hr_serial_set_baudrate(&hrs, baudrates[i]);
    if (eno != EOK) {
      printf(" -- skip\n");
      continue;
    }
    ECALL(hr_serial_set_baudrate(&hrs, baudrates[i]));
    ECALL(hr_serial_open(&hrs, dev, unit));
    run_app(argc, argv, &hrs, id);
    ECALL(hr_serial_close(&hrs));
  }

  return 0;
}
