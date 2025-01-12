/* for printf */
#include <stdio.h>

#include <stdbool.h>

#include "rsx/rsx.h"

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs) {
  EVALUE(NULL, hrs);

  uint8_t id = 0x01;

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  // ECALL(rsx_save_rom(&rsx, hrs, id));
  // ECALL(dump(&rsx));

  ECALL(rsx_reboot(&rsx, hrs, id));
  ECALL(dump(&rsx));

  // ECALL(rsx_factory_reset(&rsx, hrs, id));
  // ECALL(dump(&rsx));

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));

  char* dev  = "ttyUSB";
  char* unit = "0";
  if (argc >= 3) {
    dev  = argv[1];
    unit = argv[2];
  }
  printf("open %s %s\n", dev, unit);
  ECALL(hr_serial_open(&hrs, dev, unit));

  run_app(argc, argv, &hrs);

  ECALL(hr_serial_close(&hrs));

  return 0;
}
