/* for printf */
#include <stdio.h>

#include <stdbool.h>

// #define DATA_DUMP
#include "rsx/rsx.h"

#include "util.h"

int run_app(int argc, char *argv[], hr_serial *hrs) {
  EVALUE(NULL, hrs);

  uint8_t ids[] = {0x01, 0x02};

  rsx rsx;
  rsx_config rsx_config;
  ECALL(rsx_config_init(&rsx_config));
  rsx_config.max_payload_size = 32;
  ECALL(rsx_init(&rsx, &rsx_config));

  rsx_wait_usec(200*1000);

  for (int i = 0; i < sizeof(ids)/sizeof(ids[0]); i++) {
    printf("----------- ROM --------------\n");
    ECALL(GetROMInfo(&rsx, hrs, ids[i]));
    printf("----------- RAM --------------\n");
    ECALL(GetRAMInfo(&rsx, hrs, ids[i]));
  }

  return 0;
}

int main(int argc, char *argv[]) {
  hr_serial hrs;
  ECALL(hr_serial_init(&hrs));
  ECALL(hr_serial_set_baudrate(&hrs, 230400));

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
