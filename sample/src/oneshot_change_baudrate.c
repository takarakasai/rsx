/* for printf */
#include <stdio.h>

/* for usleep */
#include <unistd.h>

#include "rsx.h"

#include <stdbool.h>

#include "util.h"

int change_baudrate(rsx* rsx, hr_serial *hrs, uint8_t id, int baudrate) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  errno_t eno = rsx_check_connection(rsx, hrs, id);
  if (eno == EOK) {
    printf("OK\n");
  } else {
    printf("NG\n");
  }

  ECALL(rsx_set_baudrate(rsx, hrs, id, baudrate));

  ECALL(rsx_save_rom(rsx, hrs, id));

  const int max = 5;
  for (int i = 0; i < max; i++) {
    printf("waiting %d\n", i);
    sleep(1);
  }

  ECALL(rsx_reboot(rsx, hrs, id));

  sleep(1);

  return 0;
}

int check_connection(rsx* rsx, hr_serial *hrs, uint8_t id) {
  EVALUE(NULL, rsx);
  EVALUE(NULL, hrs);

  errno_t eno = rsx_check_connection(rsx, hrs, id);
  if (eno == EOK) {
    printf("OK\n");
  } else {
    printf("NG\n");
  }

  int baudrate;
  ECALL(rsx_get_baudrate(rsx, hrs, id, &baudrate));
  printf(" baudrate : %d\n", baudrate);

  return 0;
}

int main(int argc, char *argv[]) {
  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  // uint8_t id = 0x02;
  uint8_t id = 0x01;
  char* dev  = "ttyUSB";
  char* unit = "0";
  if (argc >= 3) {
    dev  = argv[1];
    unit = argv[2];
  }
  printf("open %s %s\n", dev, unit);
  ECALL(hr_serial_open(&hrs, dev, unit));

  change_baudrate(&rsx, &hrs, id, 230400);

  ECALL(hr_serial_close(&hrs));

  ECALL(hr_serial_set_baudrate(&hrs, 230400));
  ECALL(hr_serial_open(&hrs, dev, unit));

  check_connection(&rsx, &hrs, id);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
